#ifndef GLOBALS_H
#define GLOBALS_H
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#ifdef XIAO_SAMD
  #define BUZZER_PIN 0
  #define VOLTAGE_PIN 10
#else
  #define BUZZER_PIN D0
  #define VOLTAGE_PIN D10
#endif
#define VOLTAGE_DIVIDOR 2

#define DEBUGln(x) Serial.println((x))
#define DEBUG(x) Serial.print((x))

#define ZERO_LEVEL_PRESSURE 1015.0
#define ALT_FILTER 0.05
#define SPD_FILTER 0.05
#define CALIBRATION_TIME 3000

#define MIN_CLIMB_SPD 0.28
#define MIN_CLIMB_FREQ 250
#define MAX_CLIMB_SPD 1.0
#define MAX_CLIMB_FREQ 300
#define CLIMB_LCD_DEADZONE 0.15

#define MAX_DROP_SPD 0.5

#define LOOP_HZ 50
#define LCD_LOOP_SKIP 1

#define VOLTAGES_LEVELS {4150, 4110, 4080, 4020, 3980, 3950, 3910, 3870, 3850, 3840, 3820, 3800, 3790, 3770, 3750, 3730, 3710, 3690, 3610, 3300}
#define DIODE_VOLTAGE_DROP 350




float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#endif