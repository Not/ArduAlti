//#define XIAO_SAMD
#define XIAO_RP

#include <Arduino.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <Fonts/TomThumb.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
//#include <Numbers24pt.h>

#include <LCDMenuLib2.h>  

#include <Tone.h>
#include <Queue.h>
#include <Ewma.h>

#include <Globals.h>
#include <BuzzerPlayer.h>
#include <Note.h>
#include <ChargeState.h>
#include <Button.h>
#include <VarioParams.h>
#include <menu_func.h>


//XIAO nrf
#include <bluefruit.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>

using namespace Adafruit_LittleFS_Namespace;

File file(InternalFS);
Adafruit_BMP280 bmp; // I2C
BuzzerPlayer player;
VarioParams params;
Ewma alt_filter(0.05);
Ewma spd_filter(0.05);
BLEDfu  bledfu;  // OTA DFU service
BLEDis  bledis;  // device information
BLEUart bleuart; // uart over ble
BLEBas  blebas;  // battery
ChargeState charge_state;
uint8_t current_page = 0;

bool sound_on = true;
float sea_level_hpa=1013.25;
float filtered_alt=0;

Button btns[] = {Button(D6), Button(D3), Button(D1)};


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
void print_params(VarioParams p){
  Serial.println("VarioParams:");
  Serial.print("\tc_rate ");Serial.println(p.c.rate);
  Serial.print("\td_rate ");Serial.println(p.d.rate);
  Serial.print("\tc_pitch ");Serial.println(p.c.pitch);
  Serial.print("\td_pitch ");Serial.println(p.d.pitch);  
}
VarioParams read_params_from_eeprom(){
  VarioParams p;
  uint8_t *bytePtr = (uint8_t*)&p;
  if(file.open("parms", FILE_O_READ)){
      file.read(bytePtr, sizeof(p));
      file.close();
      Serial.println("Data succesfully read");
    }
    else{
      Serial.println("File open failed");
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
    if(file.open("parms", FILE_O_WRITE)){
      file.seek(0);
      file.write(bytePtr, sizeof(p));
      file.close();
      Serial.println("Data succesfully saved");
      return true;
    }
    else{
      Serial.println("File open failed");
      return false;
    } 
}

void display_temp(float temp){
    display.setFont(&TomThumb);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(95,5);
    display.print((int)temp);display.print("'C");
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
    snprintf(buffer, 18,"%02d:%02d%c",minutes, seconds, '\0');
}

void display_time(long time_ms){
    display.setFont();
    display.setCursor(32,15);
    char buffer[18]={};
    get_time(buffer, time_ms);
    display.print(buffer);
    display.drawRoundRect(22,13,65,12,3,SSD1306_WHITE);
}

void prepare_lk_frame(char* buf, float pressure, float alt, float speed, float temp, ChargeState charge_state, bool send_raw_pressure = false, bool send_raw_voltage = false ){
    int prs = send_raw_pressure?(int)pressure*100:999999;
    char bat[8] = {};
    if (send_raw_voltage)
        snprintf(bat, 8, "%.2f", charge_state.battery_voltage_mV/1000.);
    else
        snprintf(bat,8, "%d", charge_state.batter_percentage+1000);
    int j = snprintf(buf, 64, "$LK8EX1,%d,%.2f,%d,%.1f,%s,*",prs, alt, (int)(speed*100), temp, bat);


    int XOR = 0;

    for (unsigned int i = 1; i < strlen(buf)-1; i++) {
      char c = buf[i];
      if (c == '*') {break;}
      XOR ^= c;
    }
    sprintf(buf+j, "%X", XOR);

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
	void display_spd_full(float speed){
    display.setCursor(0,62);
    display.setTextSize(2);
    display.setFont(&FreeSans24pt7b);

    if (speed>CLIMB_LCD_DEADZONE){
        display.fillScreen(SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK);
    }
    else{
        display.fillScreen(SSD1306_BLACK);
        display.setTextColor(SSD1306_WHITE);
    }

    display.print(abs(speed),1);
}

void display_alt_full(float alt){
    display.setCursor(0,40);
    display.setTextSize(1);
    display.setFont(&FreeSans24pt7b);
    display.fillScreen(SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    alt = alt;
    if(alt>1000){
      display.print((int)alt);
    }else{
    display.print(alt,1);
    display.setFont();
    display.print("m");
    }
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

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.println();
  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
}

void setup_ble(){

  Bluefruit.autoConnLed(true);
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);

  Bluefruit.begin();
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  //Bluefruit.setName(getMcuUniqueID()); // useful testing with multiple central connections
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  bledfu.begin();

  // Configure and Start Device Information Service
  bledis.setManufacturer("KK");
  bledis.setModel("ArduAlti V2 BLE");
  bledis.begin();

  // Configure and Start BLE Uart Service
  bleuart.begin();

  // Start BLE Battery Service
  blebas.begin();
  blebas.write(100);

  // Set up and start advertising
  startAdv();
}


void setup() {
  setup_ble();

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    while(true){
          Serial.println(F("SSD1306 allocation failed"));     
    }
  }
  Serial.println(F("SSD1306 CONNECTED"));

  if(bmp.begin(BMP280_ADDRESS_ALT)){
    Serial.println(F("BMP CONNECTED"));
  }


  InternalFS.begin();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  //display.setRotation(2);
  display.setFont(&TomThumb);
  display.setTextColor(SSD1306_WHITE ); // Draw white text
  charge_state.begin();
  btns[0].setup();
  btns[1].setup();
  btns[2].setup();
  for (int i=0;i<3;i++){
    player.add_note(Note(i*100+100, 100, 100));
  }
  // params = get_default_params();
  // write_params_to_eeprom(params);
  params = read_params_from_eeprom();
  print_params(params);
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
  if(i%LCD_BATT_SKIP==0){
    charge_state.set_from_readings();
  }

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
      if (current_page==0){
        display_spd_gauge(filtered_spd);
        display_spd(filtered_spd);
        display_temp(temp);
        display_battery(charge_state);
        display_alt(filtered_alt);
        display_time(current_time-timer_reset_ts);
      }
      else if (current_page==1){
        display_spd_full(filtered_spd);
      }
      
    }
    charge_state.print(&Serial);
    // Serial.print(filtered_alt);
    // Serial.print(" ");
    // Serial.print(filtered_spd);
    // Serial.print(" ");
    // Serial.print(charge_state.batter_percentage);
    // Serial.print(" ");
    // Serial.println(temp);

    if (btns[0].check_pressed()==2){
      current_page++;
      current_page = current_page % PG_COUNT;
    }
  }

  //ble
  char buf[100]={};
  prepare_lk_frame(buf, 0, filtered_alt, filtered_spd, temp, charge_state);
  bleuart.write(buf, strlen(buf));
  bleuart.println();

  

  float desired_loop_time_ms = 1000/LOOP_HZ;
  delay(max(desired_loop_time_ms-(millis()-loop_start_time),0));
  display.display();

  i++;
}