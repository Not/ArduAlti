//#define XIAO_SAMD
#define XIAO_RP

#include <Arduino.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/TomThumb.h>
#include <Fonts/FreeSans18pt7b.h>
#ifdef XIAO_SAMD
  #include <Tone.h>
#endif
#include <Queue.h>
#include <Ewma.h>

#include <BuzzerPlayer.h>
#include <Globals.h>
#include <BuzzerPlayer.h>
#include <Note.h>
#include <ChargeState.h>
#include <Button.h>
#include <VarioParams.h>
#include <EEPROM.h>


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_BMP280 bmp; // I2C
BuzzerPlayer player;
VarioParams params;
Ewma alt_filter(ALT_FILTER);
Ewma spd_filter(SPD_FILTER);
#ifdef XIAO_SAMD
  Button btns[] = {Button(6), Button(3), Button(2)};
#else
  Button btns[] = {Button(D1), Button(D2), Button(D6)};
#endif

VarioParams get_default_params(){
  VarioParams p;
  p.a_rate=-5.0;
  p.a_pitch=150;
  p.b_rate=-2.0;
  p.b_pitch=250;
  p.c_rate=0.25;
  p.c_pitch=250;
  p.c_period=150;
  p.d_rate=5.0;
  p.d_pitch=1000;
  p.d_period=70;
  p.max_rate_lcd=1.0;
  p.min_rate_lcd=-1.0;
  return p;
}
VarioParams read_params_from_eeprom(){
  VarioParams p;
  uint8_t *bytePtr = (uint8_t*)&p;
  for(int i=0; i<sizeof(p);i++){
   *(bytePtr+i)=EEPROM.read(i);
  }
  return p;
}

bool write_params_to_eeprom(VarioParams p){
  uint8_t *bytePtr = (uint8_t*)&p;
  for(int i=0; i<sizeof(p);i++){
   EEPROM.write(i,*(bytePtr+i));
  }
  if (EEPROM.commit()) {
   Serial.println("EEPROM successfully committed");
   return true;
  } else {
    Serial.println("ERROR! EEPROM commit failed");
    return false;
  }

}

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
    display.setCursor(25, 26);
    display.setFont(&FreeSans18pt7b);

    if (speed>CLIMB_LCD_DEADZONE){
        display.fillRoundRect(25,0,48,30,3,SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK);
    }
    else{
        display.drawRoundRect(25,0,48,30,3,SSD1306_WHITE);
        display.setTextColor(SSD1306_WHITE);
    }

    display.print(abs(speed),1);
}

void display_spd_gauge(float speed){
    u_int8_t gauge_width = 18;
    u_int8_t gauge_height = 63;
    u_int8_t gauge_half = gauge_height/2;
    display.drawRoundRect(0,0,gauge_width,gauge_height,3, SSD1306_WHITE);
    display.drawFastHLine(5,gauge_half,gauge_width-2*5,SSD1306_WHITE); // parametrize 5, h/2, w-2*5
    display.setFont(&TomThumb);
    display.setCursor(5, 7); 
    display.setTextColor(SSD1306_WHITE);
    display.print(params.max_rate_lcd,1);

    display.setCursor(5, gauge_height-2); 
    display.print(-params.min_rate_lcd,1);

    if(speed > 0){
      u_int8_t bar_height = (int)mapfloat(speed, 0, params.max_rate_lcd, 0, gauge_half-2, true);
      display.fillRect(0, gauge_half-bar_height, gauge_width, bar_height, SSD1306_WHITE);
    }
    else{
      u_int8_t bar_height = (int)mapfloat(-speed, 0, -params.min_rate_lcd, 0, gauge_half-2, true);
      display.fillRect(0,gauge_half+1, gauge_width, bar_height-1, SSD1306_WHITE);
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
  Serial.println(F("SSD1306 CONNECTED"));

  if(bmp.begin(BMP280_ADDRESS_ALT)){
    Serial.println(F("BMP CONNECTED"));
  }
  EEPROM.begin(512);

  display.setTextSize(1);      // Normal 1:1 pixel scale
  //display.setRotation(2);
  display.setFont(&TomThumb);
  display.setTextColor(SSD1306_WHITE ); // Draw white text

  btns[0].setup();
  btns[1].setup();
  btns[2].setup();

  for (int i=0;i<3;i++){
    player.add_note(Note(i*100+100, 100, 100));
  }
  params = get_default_params();
  //write_params_to_eeprom(params);
  //params = read_params_from_eeprom();
  print_params(params);
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
  if(filtered_spd>params.c_rate){ 
    int freq = mapfloat(filtered_spd, params.c_rate, params.d_rate, params.c_pitch, params.d_pitch, true);
    int dur = (int)mapfloat(filtered_spd, params.c_rate, params.d_rate, params.c_period, params.d_period, true);
    player.add_instant_note(Note(freq, dur, dur));
  }
  if(filtered_spd<params.b_rate){
    int freq = mapfloat(filtered_spd, params.a_rate, params.b_rate, params.a_pitch, params.b_pitch, true);
    player.add_instant_note(Note(freq, 30, 0));
  }
  //player.run();

  //Battery
  int measured_voltage = map(VOLTAGE_DIVIDOR*analogRead(VOLTAGE_PIN), 0, 1024, 0, 3300);
  ChargeState charge_state = ChargeState();
  charge_state.set_from_measured_voltage(measured_voltage);
  
  
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