#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <VarioParams.h>
#include <Ewma.h>
#include <BuzzerPlayer.h>

#ifndef GLOBALS_H
#define GLOBALS_H
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#ifdef XIAO_SAMD
  #define BUZZER_PIN 0
  #define VOLTAGE_PIN 10
#else
  #define BUZZER_PIN D0
  #define VOLTAGE_PIN D3
#endif
#define VOLTAGE_DIVIDOR 2

#define DEBUGln(x) Serial.println((x))
#define DEBUG(x) Serial.print((x))


#define CALIBRATION_TIME 2000
#define CLIMB_LCD_DEADZONE 0.15



//END

#define LOOP_HZ 50
#define LCD_LOOP_SKIP 1

#define VOLTAGES_LEVELS {4150, 4110, 4080, 4020, 3980, 3950, 3910, 3870, 3850, 3840, 3820, 3800, 3790, 3770, 3750, 3730, 3710, 3690, 3610, 3300}
#define DIODE_VOLTAGE_DROP 0


extern Adafruit_SSD1306 display;
extern VarioParams params;
extern Ewma spd_filter;
extern Ewma alt_filter;
extern bool show_menu;
extern bool sound_on;
extern float sea_level_hpa;
extern float filtered_alt;




#endif