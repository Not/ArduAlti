//#define XIAO_SAMD
#define XIAO_RP

#include <Arduino.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/TomThumb.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
//#include <Numbers24pt.h>
#include <EEPROM.h>
#include <LCDMenuLib2.h>  
#ifdef XIAO_SAMD
  #include <Tone.h>
#endif
#include <Queue.h>
#include <Ewma.h>

#include <Globals.h>
#include <BuzzerPlayer.h>
#include <Note.h>
#include <ChargeState.h>
#include <Button.h>
#include <VarioParams.h>
#include <menu_func.h>


Adafruit_BMP280 bmp; // I2C
BuzzerPlayer player;
VarioParams params;
Ewma alt_filter(0.05);
Ewma spd_filter(0.05);
bool sound_on = true;
float sea_level_hpa=1013.25;
float filtered_alt=0;
#ifdef XIAO_SAMD
  Button btns[] = {Button(6), Button(3), Button(2)};
#else
  Button btns[] = {Button(D6), Button(D2), Button(D1)};
  //Button btns[] = {Button(D0), Button(D3), Button(D2)};
#endif

Adafruit_SSD1306 display(128  , 64, &Wire, -1);



extern  LCDMenuLib2_menu LCDML_0;
extern  LCDMenuLib2 LCDML;
#define _LCDML_cfg_use_ram
LCDML_addAdvanced     (0  , LCDML_0         , 1  , NULL,     ""       , mDyn_sound,  0,   _LCDML_TYPE_dynParam); 
LCDML_add             (1  , LCDML_0         , 2  , "Curve edit"       , NULL);  
  LCDML_add           (2  , LCDML_0_2       , 1  , "Information"      , mFunc_information);              
  LCDML_add           (3  , LCDML_0_2       , 2  , "Point A"          , NULL);                  
    LCDML_addAdvanced (4  , LCDML_0_2_2     , 1  , NULL,     ""       , mDyn_rate,      0,   _LCDML_TYPE_dynParam); 
    LCDML_addAdvanced (5  , LCDML_0_2_2     , 2  , NULL,     ""       , mDyn_pitch,     0,   _LCDML_TYPE_dynParam); 
    LCDML_addAdvanced (6  , LCDML_0_2_2     , 3  , NULL,     ""       , mDyn_period,    0,   _LCDML_TYPE_dynParam); 
    LCDML_add         (7  , LCDML_0_2_2     , 4  , "Back"             , mFunc_back);
  LCDML_add           (8  , LCDML_0_2       , 3  , "Point B"          , NULL);                   
    LCDML_addAdvanced (9  , LCDML_0_2_3     , 1  , NULL,     ""       , mDyn_rate,      0,   _LCDML_TYPE_dynParam); 
    LCDML_addAdvanced (10 , LCDML_0_2_3     , 2  , NULL,     ""       , mDyn_pitch,     0,   _LCDML_TYPE_dynParam); 
    LCDML_addAdvanced (11 , LCDML_0_2_3     , 3  , NULL,     ""       , mDyn_period,    0,   _LCDML_TYPE_dynParam); 
    LCDML_add         (12 , LCDML_0_2_3     , 4  , "Back"             , mFunc_back);
  LCDML_add           (13 , LCDML_0_2       , 4  , "Point C"          , NULL);                  
    LCDML_addAdvanced (14 , LCDML_0_2_4     , 1  , NULL,     ""       , mDyn_rate,      0,   _LCDML_TYPE_dynParam); 
    LCDML_addAdvanced (15 , LCDML_0_2_4     , 2  , NULL,     ""       , mDyn_pitch,     0,   _LCDML_TYPE_dynParam); 
    LCDML_addAdvanced (16 , LCDML_0_2_4     , 3  , NULL,     ""       , mDyn_period,    0,   _LCDML_TYPE_dynParam);
    LCDML_add         (17 , LCDML_0_2_4     , 4  , "Back"             , mFunc_back);
  LCDML_add           (18 , LCDML_0_2       , 5  , "Point D"          , NULL);
    LCDML_addAdvanced (19 , LCDML_0_2_5     , 1  , NULL,     ""       , mDyn_rate,      0,   _LCDML_TYPE_dynParam); 
    LCDML_addAdvanced (20 , LCDML_0_2_5     , 2  , NULL,     ""       , mDyn_pitch,     0,   _LCDML_TYPE_dynParam); 
    LCDML_addAdvanced (21 , LCDML_0_2_5     , 3  , NULL,     ""       , mDyn_period,    0,   _LCDML_TYPE_dynParam);
    LCDML_add         (22 , LCDML_0_2_5     , 4  , "Back"             , mFunc_back);
  LCDML_add           (23 , LCDML_0_2       , 6  , "Back"             , mFunc_back);
LCDML_add             (24 , LCDML_0         , 3  , "Alt. calibrate"   , NULL);     
  LCDML_addAdvanced   (25 , LCDML_0_3       , 1  , NULL,     ""       , mDyn_alt,        0,   _LCDML_TYPE_dynParam);
  LCDML_addAdvanced   (26 , LCDML_0_3       , 2  , NULL,     ""       , mDyn_hpa,        0,   _LCDML_TYPE_dynParam);
  LCDML_add           (27 , LCDML_0_3       , 3  , "Back"             , mFunc_back);
LCDML_add             (28 , LCDML_0         , 4  , "Filter Level "    , NULL);     
  LCDML_addAdvanced   (29 , LCDML_0_4       , 1  , NULL,     ""       , mDyn_alt_filter, 0,   _LCDML_TYPE_dynParam); 
  LCDML_addAdvanced   (30 , LCDML_0_4       , 2  , NULL,     ""       , mDyn_rate_filter,0,   _LCDML_TYPE_dynParam); 
  LCDML_add           (31 , LCDML_0_4       , 3  , "Back"             , mFunc_back);
LCDML_add             (32 , LCDML_0         , 5  , "Store and apply"      , mFunc_store);
LCDML_add             (33 , LCDML_0         , 6  , "Load defaults"      , mFunc_load);
LCDML_add             (34 , LCDML_0         , 7  , "Apply"      , mFunc_exit);
  

  #define _LCDML_DISP_cnt    34

  // create menu
  LCDML_createMenu(_LCDML_DISP_cnt);

unsigned long g_LCDML_DISP_press_time = 0;
unsigned long g_LCDML_DISP_total_press_time = 0;
int g_LCDML_period = 250;
void lcdml_menu_control(void)
{
  bool any_pressed = false;
  for (int i =0;i<3;i++){
    btns[i].update_state();
    any_pressed += !btns[i].state;
  }

  if (any_pressed) {
    
    if((millis() - g_LCDML_DISP_press_time) >= g_LCDML_period) {
      g_LCDML_DISP_press_time = millis(); // reset press time
      //Serial.println(g_LCDML_period);
      if (!btns[1].state) { LCDML.BT_enter(); player.add_instant_note(Note(300,15,15));}
      if (!btns[2].state) { LCDML.BT_up();    player.add_instant_note(Note(400,15,15));}
      if (!btns[0].state) { LCDML.BT_down();  player.add_instant_note(Note(400,15,15));}
      if (g_LCDML_period>50) g_LCDML_period-=10;
    }
  }
  else g_LCDML_period = 250;
}
VarioParams get_default_params(){
  VarioParams p;
  p.a.rate=-5.0;
  p.a.pitch=150;
  p.a.period=0;
  p.b.rate=-2.0;
  p.b.pitch=250;
  p.b.period=0;
  p.c.rate=0.25;
  p.c.pitch=250;
  p.c.period=150;
  p.d.rate=5.0;
  p.d.pitch=1000;
  p.d.period=70;
  p.max_rate_lcd=1.0;
  p.min_rate_lcd=-1.0;
  p.rate_filter=0.05;
  p.alt_filter=0.05;
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

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max, bool constrain=false)
{
  if (constrain) x=constrain(x,in_min,in_max);
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
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
void get_time(char * buffer, long millis){
    int minutes = millis/60000;
    int seconds = millis/1000 - minutes*60;
    sprintf(buffer, "%02d:%02d%c",minutes, seconds, '\0');
}
void display_time(long time_ms){
    display.setFont();
    display.setCursor(32,15);
    char buffer[6]={};
    get_time(buffer, time_ms);
    display.print(buffer);
    display.drawRoundRect(22,13,65,12,3,SSD1306_WHITE);
}

void display_spd(float speed){
    display.setCursor(23, 60);
    display.setFont(&FreeSans24pt7b);

    if (speed>CLIMB_LCD_DEADZONE){
        display.fillRoundRect(22,26,65,38,3,SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK);
    }
    else{
        display.drawRoundRect(22,26,65,38,3,SSD1306_WHITE);
        display.setTextColor(SSD1306_WHITE);
    }

    display.print(abs(speed),1);
}

void display_spd_gauge(float speed){
    u_int8_t gauge_width = 18;
    u_int8_t gauge_height = 64;
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
    display.setCursor(32,2);
    display.print(altitude,1); display.print(" m");
    display.drawRoundRect(22,0,65,12,3,SSD1306_WHITE);
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
  //params = get_default_params();
  //write_params_to_eeprom(params);
  params = read_params_from_eeprom();
  alt_filter.alpha=params.alt_filter;
  spd_filter.alpha=params.rate_filter;
  LCDML_setup(_LCDML_DISP_cnt);
  LCDML.MENU_enRollover();
}

float prev_filtered_alt = 0;
float prev_spd_time = 0;
unsigned long timer_reset_ts = 0;
int i=0;
bool show_menu=false;
void loop() {
  long loop_start_time = millis();
  static long first_loop_start_time = loop_start_time;

  //Temperature
  float temp =  bmp.readTemperature();

  //Altitude
  float raw_alt = 0;
  while(true){
    raw_alt =  bmp.readAltitude(sea_level_hpa);
    if (abs(raw_alt)<20000) break; else Serial.println("read failed");
  }
  filtered_alt = alt_filter.filter(raw_alt);
  
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
  if(filtered_spd>params.c.rate && sound_on){ 
    int freq = mapfloat(filtered_spd, params.c.rate, params.d.rate, params.c.pitch, params.d.pitch, true);
    int dur = (int)mapfloat(filtered_spd, params.c.rate, params.d.rate, params.c.period, params.d.period, true);
    player.add_instant_note(Note(freq, dur, dur));
  }
  if(filtered_spd<params.b.rate && sound_on){
    int freq = mapfloat(filtered_spd, params.a.rate, params.b.rate, params.a.pitch, params.b.pitch, true);
    player.add_instant_note(Note(freq, 30, 0));
  }
  player.run();

  //Battery
  int measured_voltage = map(VOLTAGE_DIVIDOR*analogRead(VOLTAGE_PIN), 0, 1024, 0, 3300);
  ChargeState charge_state = ChargeState();
  charge_state.set_from_measured_voltage(measured_voltage);

  if (btns[2].check_pressed()==2){
    timer_reset_ts = current_time;
  }
  if (btns[1].check_pressed()==2){
    if (!show_menu){
      Serial.println("entering menu");
      show_menu=true;
      LCDML.MENU_display(true);
      LCDML.init(0);
      LCDML.MENU_enRollover();
      delay(200);
    }
  } 
  
  
  //Display about 29ms
  if (show_menu){
    LCDML.loop();
  }
  else{

    
    if(i%LCD_LOOP_SKIP==0){
      display.clearDisplay();

      //display.print(btns[0].check_pressed());
      //display.print(btns[1].check_pressed());
      //display.print(btns[2].check_pressed());

      display_spd_gauge(filtered_spd);
      display_spd(filtered_spd);
      display_temp(temp);
      display_battery(charge_state);
      display_alt(filtered_alt);
      display_time(current_time-timer_reset_ts);
    }

    Serial.print(measured_voltage);
    Serial.print(" ");
    Serial.print(filtered_alt);
    Serial.print(" ");
    Serial.println(delta);
  }
  

  float desired_loop_time_ms = 1000/LOOP_HZ;
  delay(max(desired_loop_time_ms-(millis()-loop_start_time),0));
  display.display();

  i++;
}