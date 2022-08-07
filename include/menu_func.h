  #include <LCDMenuLib2.h>  

  #include <SPI.h>
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>

  
  #define _ADAFRUIT_I2C_ADR    0x3C

  #define _ADAFRUIT_I2C_ADR    0x3C
  #define _LCDML_ADAFRUIT_TEXT_COLOR WHITE 
  
  #define _LCDML_ADAFRUIT_FONT_SIZE   1   
  #define _LCDML_ADAFRUIT_FONT_W      (6*_LCDML_ADAFRUIT_FONT_SIZE)             // font width 
  #define _LCDML_ADAFRUIT_FONT_H      (8*_LCDML_ADAFRUIT_FONT_SIZE)             // font heigt 
  
  // settings for u8g lib and lcd
  #define _LCDML_ADAFRUIT_lcd_w       128            // lcd width
  #define _LCDML_ADAFRUIT_lcd_h       64             // lcd height
  
  #define OLED_RESET -1 // not used / nicht genutzt bei diesem Display


  // nothing change here
  #define _LCDML_ADAFRUIT_cols_max    (_LCDML_ADAFRUIT_lcd_w/_LCDML_ADAFRUIT_FONT_W)  
  #define _LCDML_ADAFRUIT_rows_max    (_LCDML_ADAFRUIT_lcd_h/_LCDML_ADAFRUIT_FONT_H) 

  // rows and cols 
  // when you use more rows or cols as allowed change in LCDMenuLib.h the define "_LCDML_DISP_cfg_max_rows" and "_LCDML_DISP_cfg_max_string_length"
  // the program needs more ram with this changes
  #define _LCDML_ADAFRUIT_cols        20                   // max cols
  #define _LCDML_ADAFRUIT_rows        _LCDML_ADAFRUIT_rows_max  // max rows 


  // scrollbar width
  #define _LCDML_DSIP_use_header 1

  // old defines with new content
  #define _LCDML_DISP_cols      _LCDML_ADAFRUIT_cols
  #define _LCDML_DISP_rows      _LCDML_ADAFRUIT_rows 




void lcdml_menu_display();
void lcdml_menu_clear();
void lcdml_menu_control();

void mFunc_information(uint8_t param);
void mFunc_back(uint8_t param);
void mFunc_store(uint8_t param);
void mFunc_exit(uint8_t param);
void mFunc_load(uint8_t param);
void mFunc_goToRootMenu(uint8_t param);
void mDyn_sound(uint8_t line);
void mDyn_rate(uint8_t line);
void mDyn_period(uint8_t line);
void mDyn_hpa(uint8_t line);
void mDyn_alt(uint8_t line);
void mDyn_pitch(uint8_t line);
void mDyn_alt_filter(uint8_t line);
void mDyn_rate_filter(uint8_t line);
void mFunc_screensaver(uint8_t param);
boolean COND_hide();
