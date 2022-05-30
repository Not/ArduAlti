#include <Arduino.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/TomThumb.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Tone.h>
#include <Queue.h>
#include <Ewma.h>

#include <BuzzerPlayer.h>
#include <Globals.h>
#include <BuzzerPlayer.h>
#include <Note.h>
#include <ChargeState.h>
#include <Button.h>


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_BMP280 bmp; // I2C
BuzzerPlayer player;
Ewma alt_filter(ALT_FILTER);
Ewma spd_filter(SPD_FILTER);

Button btns[] = {Button(6), Button(3), Button(2)};


void display_temp(float temp){
    display.setFont(&TomThumb);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(95,5);
    display.print((int)temp-8);display.print("'C");
}

void display_battery(ChargeState charge_state){
      display.drawRect(SCREEN_WIDTH-12, 0, 12, 5, SSD1306_WHITE);
    display.drawFastVLine(115, 1, 3, SSD1306_WHITE); 
    if(charge_state.charging){
      display.setFont(&TomThumb);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(SCREEN_WIDTH-17, 5);
      display.print("+");
      display.drawFastHLine(SCREEN_WIDTH-10,2,8,SSD1306_WHITE);

    }
    else{
      display.fillRect(SCREEN_WIDTH-11, 0, (int)charge_state.batter_percentage*10, 5, SSD1306_WHITE);
    }
}

void display_spd(float speed){
    display.setCursor(30, 31);
    display.setFont(&FreeSans18pt7b);

    if (speed>CLIMB_LCD_DEADZONE){
        display.fillRoundRect(30,5,48,30,3,SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK);
    }
    else{
        display.drawRoundRect(30,5,48,30,3,SSD1306_WHITE);
        display.setTextColor(SSD1306_WHITE);
    }

    display.print(abs(speed),1);
}

void display_spd_gauge(float speed){
    int gauge_width = 18;
    int gauge_height = 54;
    display.drawRoundRect(0,0,gauge_width,gauge_height,3, SSD1306_WHITE);
    display.drawFastHLine(5,26,7,SSD1306_WHITE); // parametrize 5, h/2, w-2*5
    display.setFont(&TomThumb);
    display.setCursor(5, 7); 
    display.setTextColor(SSD1306_WHITE);
    display.print(MAX_CLIMB_SPD,1);

    display.setCursor(5, gauge_height-2); 
    display.print(MAX_DROP_SPD,1);

    if(speed > 0){
      u_int8_t bar_height = (int)mapfloat(speed, 0, MAX_CLIMB_SPD, 0, 25);
      display.fillRect(0, 26-bar_height, gauge_width, bar_height, SSD1306_WHITE);
    }
    else{
      u_int8_t bar_height = (int)mapfloat(-speed, 0, MAX_DROP_SPD, 0, 25);
      display.fillRect(0,27, gauge_width, bar_height-1, SSD1306_WHITE);
    }
}

void display_alt(float altitude){
    display.setFont();
    display.setCursor(31,38);
    display.print(altitude,1); display.print(" m");
    display.display();
}



void setup() {

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    while(true){
          Serial.println(F("SSD1306 allocation failed"));
    }
  }

  if(bmp.begin(BMP280_ADDRESS_ALT)){
  }

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setRotation(2);
  display.setFont(&TomThumb);
  display.setTextColor(SSD1306_WHITE ); // Draw white text

  for (auto btn: btns){
    btn.setup();
  }

  for (int i=0;i<3;i++){
    player.add_note(Note(i*100+100, 100, 100));
  }
}

float prev_filtered_alt = 0;
float prev_spd_time = 0;
int i=0;
void loop() {
  long loop_start_time = millis();
  static long first_loop_start_time = loop_start_time;

  //Temperature
  float temp = bmp.readTemperature();

  //Altitude
  float raw_alt = bmp.readAltitude(ZERO_LEVEL_PRESSURE);
  float filtered_alt =alt_filter.filter(raw_alt);
  
  //Speed
  float current_time = millis();
  int delta = current_time - prev_spd_time;
  float alt_diff = filtered_alt - prev_filtered_alt;
  if (current_time - first_loop_start_time < CALIBRATION_TIME) alt_diff = 0;
  float raw_spd = 1000.0*alt_diff/delta;
  float filtered_spd = spd_filter.filter(raw_spd);

  prev_spd_time=millis();
  prev_filtered_alt = filtered_alt;

  //Sound
  int freq = mapfloat(filtered_spd, MIN_CLIMB_SPD, MAX_CLIMB_SPD, MIN_CLIMB_FREQ, MAX_CLIMB_FREQ);
  int dur = (int)mapfloat(filtered_spd, MIN_CLIMB_SPD, MAX_CLIMB_SPD, 150, 70);
  if (filtered_spd > MIN_CLIMB_SPD) player.add_instant_note(Note(freq, dur, dur));
  //player.run();

  //Battery
  int measured_voltage = map(VOLTAGE_DIVIDOR*analogRead(VOLTAGE_PIN), 0, 1024, 0, 3300);
  ChargeState charge_state = ChargeState();
  charge_state.set_from_measured_voltage(measured_voltage);
  
  //Buttons
  




  //Display about 29ms
  if(i%LCD_LOOP_SKIP==0){
    display.clearDisplay();
    display.setCursor(80,40);

    display.print(btns[0].check_pressed());
    display.print(btns[1].check_pressed());
    display.print(btns[2].check_pressed());

    display_spd_gauge(filtered_spd);
    display_spd(filtered_spd);
    display_temp(temp);
    display_battery(charge_state);
    display_alt(filtered_alt);


  }

  Serial.print(filtered_spd);
  Serial.print(" ");
  Serial.println(delta);
  

  float desired_loop_time_ms = 1000/LOOP_HZ;
  delay(max(desired_loop_time_ms-(millis()-loop_start_time),0));

  i++;


}