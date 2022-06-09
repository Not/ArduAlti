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

#include <LCDMenuLib2.h>


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_BMP280 bmp; // I2C
BuzzerPlayer player;
Ewma alt_filter(ALT_FILTER);
Ewma spd_filter(SPD_FILTER);
Button btns[] = {Button(6), Button(3), Button(2)};


void lcdml_menu_display();
void lcdml_menu_clear();
void lcdml_menu_control();

void mFunc_information(uint8_t param);
void mFunc_timer_info(uint8_t param);
void mFunc_back(uint8_t param);
void mFunc_p2(uint8_t param);
void mFunc_goToRootMenu(uint8_t param);
void mFunc_jumpTo_timer_info(uint8_t param);
void mFunc_para(uint8_t param);
void mDyn_para(uint8_t line);

LCDMenuLib2_menu LCDML_0 (255, 0, 0, NULL, NULL); // root menu element (do not change)
LCDMenuLib2 LCDML(LCDML_0, _LCDML_DISP_rows-_LCDML_DSIP_use_header, _LCDML_DISP_cols, lcdml_menu_display, lcdml_menu_clear, lcdml_menu_control);

LCDML_add         (0  , LCDML_0         , 1  , "Information"      , mFunc_information);       // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add         (1  , LCDML_0         , 2  , "Time info"        , mFunc_timer_info);        // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add         (2  , LCDML_0         , 3  , "Program"          , NULL);                    // NULL = no menu function
LCDML_add         (3  , LCDML_0_3       , 1  , "Program 1"        , NULL);                    // NULL = no menu function
LCDML_add         (4  , LCDML_0_3_1     , 1  , "P1 dummy"         , NULL);                    // NULL = no menu function
LCDML_add         (5  , LCDML_0_3_1     , 2  , "P1 Settings"      , NULL);                    // NULL = no menu function
LCDML_add         (6  , LCDML_0_3_1_2   , 1  , "Warm"             , NULL);                    // NULL = no menu function
LCDML_add         (7  , LCDML_0_3_1_2   , 2  , "Cold"             , NULL);                    // NULL = no menu function
LCDML_add         (8  , LCDML_0_3_1_2   , 3  , "Back"             , mFunc_back);              // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add         (9  , LCDML_0_3_1     , 3  , "Back"             , mFunc_back);              // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add         (10 , LCDML_0_3       , 2  , "Program 2"        , mFunc_p2);                // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add         (11 , LCDML_0_3       , 3  , "Back"             , mFunc_back);              // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add         (12 , LCDML_0         , 4  , "Special"          , NULL);                    // NULL = no menu function
LCDML_add         (13 , LCDML_0_4       , 1  , "Go to Root"       , mFunc_goToRootMenu);      // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add         (14 , LCDML_0_4       , 2  , "Jump to Time info", mFunc_jumpTo_timer_info); // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add         (15 , LCDML_0_4       , 3  , "Back"             , mFunc_back);              // this menu function can be found on "LCDML_display_menuFunction" tab

// Advanced menu (for profit) part with more settings
// Example for one function and different parameters
// It is recommend to use parameters for switching settings like, (small drink, medium drink, big drink) or (200ml, 400ml, 600ml, 800ml) ...
// the parameter change can also be released with dynParams on the next example
// LCDMenuLib_addAdvanced(id, prev_layer,     new_num, condition,   lang_char_array, callback_function, parameter (0-255), menu function type  )
LCDML_addAdvanced (16 , LCDML_0         , 5  , NULL,          "Parameter"      , NULL,                0,            _LCDML_TYPE_default);                    // NULL = no menu function
LCDML_addAdvanced (17 , LCDML_0_5       , 1  , NULL,          "Parameter 1"      , mFunc_para,       10,            _LCDML_TYPE_default);                    // NULL = no menu function
LCDML_addAdvanced (18 , LCDML_0_5       , 2  , NULL,          "Parameter 2"      , mFunc_para,       20,            _LCDML_TYPE_default);                    // NULL = no menu function
LCDML_addAdvanced (19 , LCDML_0_5       , 3  , NULL,          "Parameter 3"      , mFunc_para,       30,            _LCDML_TYPE_default);                    // NULL = no menu function
LCDML_add         (20 , LCDML_0_5       , 4  , "Back"             , mFunc_back);              // this menu function can be found on "LCDML_display_menuFunction" tab

LCDML_addAdvanced (21 , LCDML_0         , 6  , NULL,          ""                  , mDyn_para,                0,   _LCDML_TYPE_dynParam);                     // NULL = no menu function

#define _LCDML_DISP_cnt    21
LCDML_createMenu(_LCDML_DISP_cnt);



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
}



void setup() {
  Serial.begin(9600);

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
  LCDML_setup(_LCDML_DISP_cnt);

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

    // display.print(btns[0].check_pressed());
    // display.print(btns[1].check_pressed());
    // display.print(btns[2].check_pressed());

    // display_spd_gauge(filtered_spd);
    // display_spd(filtered_spd);
    // display_temp(temp);
    // display_battery(charge_state);
    // display_alt(filtered_alt);
    display.display();


  }

  // //Serial.print(filtered_spd);
  // //Serial.print(" ");
  // //Serial.println(delta);
  

  //float desired_loop_time_ms = 1000/LOOP_HZ;
  //delay(max(desired_loop_time_ms-(millis()-loop_start_time),0));
   if (digitalRead(3) == 0){
     LCDML.loop();
   }
  //i++;


}

//control
# define _LCDML_CONTROL_serial_enter           'e'
# define _LCDML_CONTROL_serial_up              'w'
# define _LCDML_CONTROL_serial_down            's'
# define _LCDML_CONTROL_serial_left            'a'
# define _LCDML_CONTROL_serial_right           'd'
# define _LCDML_CONTROL_serial_quit            'q'

// example for the useage of events (not needed everywhere)
// this defines are only for examples and can be renamed
# define _LCDML_EVENT_command                'c'
# define _LCDML_EVENT_char_0                 '0'
# define _LCDML_EVENT_char_1                 '1'
# define _LCDML_EVENT_char_2                 '2'
# define _LCDML_EVENT_char_3                 '3'
# define _LCDML_EVENT_char_4                 '4'
# define _LCDML_EVENT_char_5                 '5'
# define _LCDML_EVENT_char_6                 '6'
# define _LCDML_EVENT_char_7                 '7'
# define _LCDML_EVENT_char_8                 '8'
# define _LCDML_EVENT_char_9                 '9'
// *****
void lcdml_menu_control(void)
{
  // If something must init, put in in the setup condition
  if(LCDML.BT_setup()) {
    // runs only once 
  }

  if(LCDML.CE_setup()) {
    // runs only once
  }

  // check if new serial input is available
  if (Serial.available()) {
    // read one char from input buffer    
    switch (Serial.read())
    {
      case _LCDML_CONTROL_serial_enter:  LCDML.BT_enter(); break;
      case _LCDML_CONTROL_serial_up:     LCDML.BT_up();    break;
      case _LCDML_CONTROL_serial_down:   LCDML.BT_down();  break;
      case _LCDML_CONTROL_serial_left:   LCDML.BT_left();  break;
      case _LCDML_CONTROL_serial_right:  LCDML.BT_right(); break;
      case _LCDML_CONTROL_serial_quit:   LCDML.BT_quit();  break;
      // example for event handling 
      // custom event handling
      // is is also possible to enable more the one event on the same time
      // but when more then one events with callback functions are active 
      // only the first callback function is called. (first = by number)
      case _LCDML_EVENT_command:  LCDML.CE_set(0);   break;
      case _LCDML_EVENT_char_0:   LCDML.CE_set(1);   break;
      case _LCDML_EVENT_char_1:   LCDML.CE_set(2);   break;
      case _LCDML_EVENT_char_2:   LCDML.CE_set(3);   break;
      case _LCDML_EVENT_char_3:   LCDML.CE_set(4);   break;
      case _LCDML_EVENT_char_4:   LCDML.CE_set(5);   break;
      case _LCDML_EVENT_char_5:   LCDML.CE_set(6);   break;
      case _LCDML_EVENT_char_6:   LCDML.CE_set(7);   break;
      case _LCDML_EVENT_char_7:   LCDML.CE_set(8);   break;
      case _LCDML_EVENT_char_8:   LCDML.CE_set(9);   break;
      case _LCDML_EVENT_char_9:   LCDML.CE_set(10);  break;
      default: break;
    }
  }
}



uint8_t g_dynParam = 100; // when this value comes from an EEPROM, load it in setup
                          // at the moment here is no setup function (To-Do)
void mDyn_para(uint8_t line)
// *********************************************************************
{
  // check if this function is active (cursor stands on this line)
  if (line == LCDML.MENU_getCursorPos())
  {
    // make only an action when the cursor stands on this menu item
    //check Button
    if(LCDML.BT_checkAny())
    {
      if(LCDML.BT_checkEnter())
      {
        // this function checks returns the scroll disable status (0 = menu scrolling enabled, 1 = menu scrolling disabled)
        if(LCDML.MENU_getScrollDisableStatus() == 0)
        {
          // disable the menu scroll function to catch the cursor on this point
          // now it is possible to work with BT_checkUp and BT_checkDown in this function
          // this function can only be called in a menu, not in a menu function
          LCDML.MENU_disScroll();
        }
        else
        {
          // enable the normal menu scroll function
          LCDML.MENU_enScroll();
        }

        // do something
        // ...
      }

      // This check have only an effect when MENU_disScroll is set
      if(LCDML.BT_checkUp())
      {
        g_dynParam++;
      }

      // This check have only an effect when MENU_disScroll is set
      if(LCDML.BT_checkDown())
      {
        g_dynParam--;
      }


      if(LCDML.BT_checkLeft())
      {
        g_dynParam++;
      }
      if(LCDML.BT_checkRight())
      {
        g_dynParam--;
      }
    }
  }

  Serial.print("dynValue:");
  Serial.print(g_dynParam);
}

// =====================================================================
//
// Output function
//
// =====================================================================

/* ******************************************************************** */
void lcdml_menu_clear()
/* ******************************************************************** */
{
  for(uint8_t i=0;i<15;i++) {
    Serial.println();
  }
}

/* ******************************************************************** */
void lcdml_menu_display()
/* ******************************************************************** */
{
  // init vars
  //uint8_t n_max = (LCDML.MENU_getChilds() >= _LCDML_DISP_rows) ? _LCDML_DISP_rows : (LCDML.MENU_getChilds());

  // update content
  // ***************
  if (LCDML.DISP_checkMenuUpdate() || LCDML.DISP_checkMenuCursorUpdate() ) {
    // clear menu
    // ***************
    LCDML.DISP_clear();

    Serial.println(F("==========================================="));
    Serial.println(F("================  var ===================="));
    Serial.println(F("==========================================="));

    // declaration of some variables
    // ***************
    // content variable
    char content_text[_LCDML_DISP_cols];  // save the content text of every menu element
    // menu element object
    LCDMenuLib2_menu *tmp;
    // some limit values
    uint8_t i = LCDML.MENU_getScroll();
    uint8_t maxi = (_LCDML_DISP_rows - _LCDML_DSIP_use_header) + i;
    uint8_t n = 0;

    // check if this element has children
    if ((tmp = LCDML.MENU_getDisplayedObj()) != NULL)
    {
      // Display a header with the parent element name
      if(_LCDML_DSIP_use_header > 0)
      {
        // only one line
        if(LCDML.MENU_getLayer() == 0)
        {
          // this text is displayed when no header is available
          Serial.println(F("Root Menu"));
        }
        else
        {
          // Display parent name
          LCDML_getContent(content_text, LCDML.MENU_getParentID());
          Serial.print(content_text);
          Serial.println();
        }        
      }

      // loop to display lines
      do
      {
        // check if a menu element has a condition and if the condition be true
        if (tmp->checkCondition())
        {
          // display cursor
          if (n == LCDML.MENU_getCursorPos())
          {
            Serial.print(F("(x) "));
          }
          else
          {
            Serial.print(F("( ) "));
          }

          // check the type off a menu element
          if(tmp->checkType_menu() == true)
          {
            // display normal content
            LCDML_getContent(content_text, tmp->getID());
            Serial.print(content_text);
          }
          else
          {
            if(tmp->checkType_dynParam()) {
              tmp->callback(n);
            }
          }

          Serial.println();

          // increment some values
          i++;
          n++;
        }
      // try to go to the next sibling and check the number of displayed rows
      } while (((tmp = tmp->getSibling(1)) != NULL) && (i < maxi));
    }
  }
}
  // *********************************************************************
void mFunc_information(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);

    // setup function
    Serial.println(F("==========================================="));
    Serial.println(F("================  FUNC ===================="));
    Serial.println(F("==========================================="));

    Serial.println(F("To close this"));
    Serial.println(F("function press"));
    Serial.println(F("any button or use"));
    Serial.println(F("back button"));
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
    // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
    // the quit button works in every DISP function without any checks; it starts the loop_end function
    if(LCDML.BT_checkAny()) { // check if any button is pressed (enter, up, down, left, right)
      // LCDML_goToMenu stops a running menu function and goes to the menu
      LCDML.FUNC_goBackToMenu();
    }
  }

  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}


// *********************************************************************
uint8_t g_func_timer_info = 0;  // time counter (global variable)
unsigned long g_timer_1 = 0;    // timer variable (global variable)
void mFunc_timer_info(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);

    Serial.println(F("==========================================="));
    Serial.println(F("================  FUNC ===================="));
    Serial.println(F("==========================================="));
    Serial.println(F("wait 20 seconds or press back button"));
    g_func_timer_info = 20;       // reset and set timer
    LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds

    LCDML.TIMER_msReset(g_timer_1);
  }


  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
    // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
    // the quit button works in every DISP function without any checks; it starts the loop_end function

    // reset screensaver timer
    LCDML.SCREEN_resetTimer();

    // this function is called every 100 milliseconds

    // this method checks every 1000 milliseconds if it is called
    if(LCDML.TIMER_ms(g_timer_1, 1000)) {
      g_func_timer_info--;                // increment the value every second
      Serial.println(g_func_timer_info);  // print the time counter value
    }

    // this function can only be ended when quit button is pressed or the time is over
    // check if the function ends normally
    if (g_func_timer_info <= 0)
    {
      // leave this function
      LCDML.FUNC_goBackToMenu();
    }
  }

  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}


// *********************************************************************
uint8_t g_button_value = 0; // button value counter (global variable)
void mFunc_p2(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);

    // setup function
    // print LCD content
    Serial.println(F("==========================================="));
    Serial.println(F("================  FUNC ===================="));
    Serial.println(F("==========================================="));
    Serial.println(F("press a or w"));
    Serial.println(F("count: 0 of 3"));
    // Reset Button Value
    g_button_value = 0;

    // Disable the screensaver for this function until it is closed
    LCDML.FUNC_disableScreensaver();

  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
    // the quit button works in every DISP function without any checks; it starts the loop_end function
    if (LCDML.BT_checkAny()) // check if any button is pressed (enter, up, down, left, right)
    {
      if (LCDML.BT_checkLeft() || LCDML.BT_checkUp()) // check if button left is pressed
      {
        LCDML.BT_resetLeft(); // reset the left button
        LCDML.BT_resetUp(); // reset the left button
        g_button_value++;

        // update LCD content
        Serial.print(F("count: "));
        Serial.print(g_button_value); //print change content
        Serial.println(F(" of 3"));
      }
    }

    // check if button count is three
    if (g_button_value >= 3) {
      LCDML.FUNC_goBackToMenu();      // leave this function
    }
  }

  if(LCDML.FUNC_close())     // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}


// *********************************************************************
void mFunc_back(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);

    // end function and go an layer back
    LCDML.FUNC_goBackToMenu(1);      // leave this function
  }
}


// *********************************************************************
void mFunc_screensaver(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);

    Serial.println(F("start screensaver"));
    Serial.println(F("press any key to quit"));
    LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds
  }

  if(LCDML.FUNC_loop())
  {
    if (LCDML.BT_checkAny()) // check if any button is pressed (enter, up, down, left, right)
    {
      LCDML.FUNC_goBackToMenu();
    }
  }

  if(LCDML.FUNC_close())
  {
     LCDML.MENU_goRoot();
  }
}


// *********************************************************************
void mFunc_goToRootMenu(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);

    // go to root and display menu
    LCDML.MENU_goRoot();
  }
}

// *********************************************************************
void mFunc_jumpTo_timer_info(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);
    
    // Jump to main screen
    LCDML.OTHER_jumpToFunc(mFunc_timer_info);
  }
}


// *********************************************************************
void mFunc_para(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    Serial.println(F("==========================================="));
    Serial.println(F("================  FUNC ===================="));
    Serial.println(F("==========================================="));
    Serial.print(F("parameter: "));
    Serial.println(param);

    Serial.println(F("press any key"));
    Serial.println(F("to leave it"));

    LCDML.FUNC_setLoopInterval(100);  // starts a trigger event for the loop function every 100 milliseconds
  }

  if(LCDML.FUNC_loop())          // ****** LOOP *********
  {
    // For example
    switch(param)
    {
      case 10:
        // do something
        break;

      case 20:
        // do something
        break;

      case 30:
        // do something
        break;

      default:
        // do nothing
        break;
    }


    if (LCDML.BT_checkAny()) // check if any button is pressed (enter, up, down, left, right)
    {
      LCDML.FUNC_goBackToMenu();  // leave this function
    }
  }

  if(LCDML.FUNC_close())        // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}


// *********************************************************************
void mFunc_exampleEventHandling(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    LCDML_UNUSED(param);
    
    // setup
    // is called only if it is started

    // starts a trigger event for the loop function every 100 milliseconds
    //LCDML.FUNC_setLoopInterval(100);

    // uncomment this line when the menu should go back to the last called position
    // this could be a cursor position or the an active menu function
    // GBA means => go back advanced
    LCDML.FUNC_setGBA(); 

    // display content
    Serial.println("Event handling function is active");
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
    // loop
    // is called when it is triggered
    // - with LCDML_DISP_triggerMenu( milliseconds )
    // - with every button status change

    // uncomment this line when the screensaver should not be called when this function is running
    // reset screensaver timer
    // this function is only working when a loop timer is set
    //LCDML.SCREEN_resetTimer();

    // check if any button is pressed (enter, up, down, left, right)
    if(LCDML.BT_checkAny()) {
      LCDML.FUNC_goBackToMenu();
    }   

    // check if any event is active  
    if(LCDML.CE_checkAny())
    { 
      // check if the defined command char is pressed 
      if(LCDML.CE_check(0))
      {
        Serial.println(F("The defined command char is enabled"));         
      }
      
      // check if a special char was pressed
      // there are 10 defined special chars in control tab
      for(uint8_t i=1; i<=10; i++)
      {
        if(LCDML.CE_check(i))
        {
          Serial.print(F("The command number: "));
          Serial.print(i-1);
          Serial.println(F(" is pressed"));
        }
      }        
    }
    else
    {
      Serial.println(F("This function is called by menu, not over the callback function")); 
      Serial.println(F("Press 'c' or a number to display the content"));
    }
    LCDML.CE_resetAll();
  }

  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // loop end
    // you can here reset some global vars or delete it

    // reset all events 
    LCDML.CE_resetAll();
  }
}