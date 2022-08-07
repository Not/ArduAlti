#include <LCDMenuLib2.h>  
#include <Globals.h>
#include<menu_func.h>

LCDMenuLib2_menu LCDML_0 (255, 0, 0, NULL, NULL); // root menu element (do not change)
LCDMenuLib2 LCDML(LCDML_0, _LCDML_DISP_rows-_LCDML_DSIP_use_header, _LCDML_DISP_cols, lcdml_menu_display, lcdml_menu_clear, lcdml_menu_control);
void write_params_to_eeprom(VarioParams p);
VarioParams get_default_params();
extern BuzzerPlayer player;


void mDyn_sound(uint8_t line)
{
  char buf[20];
  if (line == LCDML.MENU_getCursorPos())
  {
    char content_text[_LCDML_DISP_cols];
    LCDML_getContent(content_text, LCDML.MENU_getParentID());
    Serial.println(content_text);
    if(LCDML.BT_checkAny())
    {
      
      // This check have only an effect when MENU_disScroll is set
      if(LCDML.BT_checkUp() || LCDML.BT_checkDown())
      {
        sound_on=!sound_on;
        LCDML.BT_resetUp();
        LCDML.BT_resetDown();
      }

      if(LCDML.BT_checkEnter())
      {
        if(LCDML.MENU_getScrollDisableStatus() == 0) LCDML.MENU_disScroll();
        else LCDML.MENU_enScroll();
        LCDML.BT_resetEnter();
      }

    }
  }

  display.setCursor(6, _LCDML_ADAFRUIT_FONT_H * (line+_LCDML_DSIP_use_header));
  display.print("Sound: ");
  if (LCDML.MENU_getScrollDisableStatus() == 0){
    if(sound_on) display.print("ON");
    else display.print("OFF");
  }
  else{
    if(sound_on) display.print("<ON>");
    else display.print("<OFF>");
  }
}
float * get_rate_for_a_point(char c){
    switch (c)
    {
    case 'A':
        return &params.a.rate;
        break;
    case 'B':
        return &params.b.rate;
        break;
    case 'C':
        return &params.c.rate;
        break;
    case 'D':
        return &params.d.rate;
        break;    
    default:
        return NULL;
        Serial.println("ERROR");
        break;
    }
}
u_int32_t * get_pitch_for_a_point(char c){
    switch (c)
    {
    case 'A':
        return &params.a.pitch;
        break;
    case 'B':
        return &params.b.pitch;
        break;
    case 'C':
        return &params.c.pitch;
        break;
    case 'D':
        return &params.d.pitch;
        break;    
    default:
        return NULL;
        Serial.println("ERROR");
        break;
    }
}
u_int32_t * get_period_for_a_point(char c){
    switch (c)
    {
    case 'A':
        return &params.a.period;
        break;
    case 'B':
        return &params.b.period;
        break;
    case 'C':
        return &params.c.period;
        break;
    case 'D':
        return &params.d.period;
        break;    
    default:
        return NULL;
        Serial.println("ERROR");
        break;
    }
}
void mDyn_alt_filter(uint8_t line){
     bool active = (line == LCDML.MENU_getCursorPos());
  if (active)
  {
    
    if(LCDML.BT_checkAny())
    {
      // This check have only an effect when MENU_disScroll is set
      if(LCDML.BT_checkUp())
      {
        (params.alt_filter)+=0.01;
        LCDML.BT_resetUp();
      }
      if(LCDML.BT_checkDown())
      {
        (params.alt_filter)-=0.01;
        LCDML.BT_resetDown();
      }
      if(LCDML.BT_checkEnter())
      {
        if(LCDML.MENU_getScrollDisableStatus() == 0) LCDML.MENU_disScroll();
        else LCDML.MENU_enScroll();
        LCDML.BT_resetEnter();
      }
    }
  }
 
  display.setCursor(6, _LCDML_ADAFRUIT_FONT_H * (line+_LCDML_DSIP_use_header));
  display.print("alt ");
  if (LCDML.MENU_getScrollDisableStatus() == 1 && active) display.print("<"); else display.print(" ");
  display.print(params.alt_filter);
  if (LCDML.MENU_getScrollDisableStatus() == 1 && active) display.print(">"); else display.print(" ");
}

void mDyn_rate_filter(uint8_t line){
     bool active = (line == LCDML.MENU_getCursorPos());
  if (active)
  {
    if(LCDML.BT_checkAny())
    {
      if(LCDML.BT_checkUp())
      {
        (params.rate_filter)+=0.01;
        LCDML.BT_resetUp();
      }
      if(LCDML.BT_checkDown())
      {
        (params.rate_filter)-=0.01;
        LCDML.BT_resetDown();
      }
      if(LCDML.BT_checkEnter())
      {
        if(LCDML.MENU_getScrollDisableStatus() == 0) LCDML.MENU_disScroll();
        else LCDML.MENU_enScroll();
        LCDML.BT_resetEnter();
      }
    }
  }
  display.setCursor(6, _LCDML_ADAFRUIT_FONT_H * (line+_LCDML_DSIP_use_header));
  display.print("rate ");
  if (LCDML.MENU_getScrollDisableStatus() == 1 && active) display.print("<"); else display.print(" ");
  display.print(params.rate_filter);
  if (LCDML.MENU_getScrollDisableStatus() == 1 && active) display.print(">"); else display.print(" ");
}

void mDyn_rate(uint8_t line)
{
  char parent_name[_LCDML_DISP_cols]={};
  LCDML_getContent(parent_name, LCDML.MENU_getParentID());
  char name[_LCDML_DISP_cols]={};
  LCDML_getContent(name, LCDML.MENU_getCurrentObj()->getID());
  Serial.println(name);
  uint8_t n = strnlen_P(parent_name,_LCDML_DISP_cols)-1;
  float * value_to_change=get_rate_for_a_point(parent_name[n]);

  bool active = (line == LCDML.MENU_getCursorPos());
  if (active)
  {
    if(LCDML.BT_checkAny())
    {
      // This check have only an effect when MENU_disScroll is set
      if(LCDML.BT_checkUp())
      {
        (*value_to_change)+=0.1;
        LCDML.BT_resetUp();
      }
      if(LCDML.BT_checkDown())
      {
        (*value_to_change)-=0.1;
        LCDML.BT_resetDown();
      }
      if(LCDML.BT_checkEnter())
      {
        if(LCDML.MENU_getScrollDisableStatus() == 0) LCDML.MENU_disScroll();
        else LCDML.MENU_enScroll();
        LCDML.BT_resetEnter();
      }
    }
  }
 
  display.setCursor(6, _LCDML_ADAFRUIT_FONT_H * (line+_LCDML_DSIP_use_header));
  display.print("rate ");
  if (LCDML.MENU_getScrollDisableStatus() == 1 && active) display.print("<"); else display.print(" ");
  display.print(*value_to_change);
  if (LCDML.MENU_getScrollDisableStatus() == 1 && active) display.print(">"); else display.print(" ");
}
void mDyn_pitch(uint8_t line)
{
  char content_text[_LCDML_DISP_cols]={};
  LCDML_getContent(content_text, LCDML.MENU_getParentID());
  uint8_t n = strnlen_P(content_text,_LCDML_DISP_cols)-1;
  uint32_t * value_to_change=get_pitch_for_a_point(content_text[n]);
  bool active = (line == LCDML.MENU_getCursorPos());
  if (active)
  {
    LCDML_getContent(content_text, LCDML.MENU_getParentID());
    if(LCDML.BT_checkAny())
    {
      if(LCDML.BT_checkUp())
      {
        (*value_to_change)+=5;
        LCDML.BT_resetUp();
      }
      if(LCDML.BT_checkDown())
      {
        (*value_to_change)-=5;
        LCDML.BT_resetDown();
      }
      if(LCDML.BT_checkEnter())
      {
        if(LCDML.MENU_getScrollDisableStatus() == 0) LCDML.MENU_disScroll();
        else LCDML.MENU_enScroll();
        LCDML.BT_resetEnter();
      }
    }
  }
 
  display.setCursor(6, _LCDML_ADAFRUIT_FONT_H * (line+_LCDML_DSIP_use_header));
  display.print("pitch ");
  if (LCDML.MENU_getScrollDisableStatus() == 1 && active) display.print("<"); else display.print(" ");
  display.print(*value_to_change);
    if (LCDML.MENU_getScrollDisableStatus() == 1 && active) display.print(">"); else display.print(" ");
}
void mDyn_period(uint8_t line)
{
  char content_text[_LCDML_DISP_cols]={};
  LCDML_getContent(content_text, LCDML.MENU_getParentID());
  uint8_t n = strnlen_P(content_text,_LCDML_DISP_cols)-1;
  uint32_t * value_to_change=get_period_for_a_point(content_text[n]);
  bool active = (line == LCDML.MENU_getCursorPos());
  if (active)
  {
    LCDML_getContent(content_text, LCDML.MENU_getParentID());
    if(LCDML.BT_checkAny())
    {
      
      // This check have only an effect when MENU_disScroll is set
      if(LCDML.BT_checkUp())
      {

        (*value_to_change)+=5;
        LCDML.BT_resetUp();
      }
      if(LCDML.BT_checkDown())
      {
        (*value_to_change)-=5;
        LCDML.BT_resetDown();
      }
      if(LCDML.BT_checkEnter())
      {
        if(LCDML.MENU_getScrollDisableStatus() == 0) LCDML.MENU_disScroll();
        else LCDML.MENU_enScroll();
        LCDML.BT_resetEnter();
      }
    }
  }
 
  display.setCursor(6, _LCDML_ADAFRUIT_FONT_H * (line+_LCDML_DSIP_use_header));
  display.print("period ");
  if (LCDML.MENU_getScrollDisableStatus() == 1 && active) display.print("<"); else display.print(" ");
  display.print(*value_to_change);
  if (LCDML.MENU_getScrollDisableStatus() == 1 && active) display.print(">"); else display.print(" ");
}

void mDyn_hpa(uint8_t line)
{
  
  bool active = (line == LCDML.MENU_getCursorPos());
  if (active)
  {
    if(LCDML.BT_checkAny())
    {
      // This check have only an effect when MENU_disScroll is set
      if(LCDML.BT_checkUp())
      {

        sea_level_hpa+=0.5;
        LCDML.BT_resetUp();
      }
      if(LCDML.BT_checkDown())
      {
        sea_level_hpa+=0.5;
        LCDML.BT_resetDown();
      }
      if(LCDML.BT_checkEnter())
      {
        if(LCDML.MENU_getScrollDisableStatus() == 0) LCDML.MENU_disScroll();
        else LCDML.MENU_enScroll();
        LCDML.BT_resetEnter();
      }
    }
  }
 
  display.setCursor(6, _LCDML_ADAFRUIT_FONT_H * (line+_LCDML_DSIP_use_header));
  display.print("0m hPa ");
  if (LCDML.MENU_getScrollDisableStatus() == 1 && active) display.print("<"); else display.print(" ");
  display.print(sea_level_hpa);
  if (LCDML.MENU_getScrollDisableStatus() == 1 && active) display.print(">"); else display.print(" ");
}

void mDyn_alt(uint8_t line)
{
  
  bool active = (line == LCDML.MENU_getCursorPos());
  if (active)
  {
    if(LCDML.BT_checkAny())
    {
      // This check have only an effect when MENU_disScroll is set
      if(LCDML.BT_checkUp())
      {

        //sea_level_hpa+=0.5;
        LCDML.BT_resetUp();
      }
      if(LCDML.BT_checkDown())
      {
        //sea_level_hpa+=0.5;
        LCDML.BT_resetDown();
      }
      if(LCDML.BT_checkEnter())
      {
        if(LCDML.MENU_getScrollDisableStatus() == 0) LCDML.MENU_disScroll();
        else LCDML.MENU_enScroll();
        LCDML.BT_resetEnter();
      }
    }
  }
 
  display.setCursor(6, _LCDML_ADAFRUIT_FONT_H * (line+_LCDML_DSIP_use_header));
  display.print("period ");
  if (LCDML.MENU_getScrollDisableStatus() == 1 && active) display.print("<"); else display.print(" ");
  display.print(filtered_alt);
  if (LCDML.MENU_getScrollDisableStatus() == 1 && active) display.print(">"); else display.print(" ");
}


void lcdml_menu_clear()
{
}

void lcdml_menu_display()
{
  // clear lcd
  display.clearDisplay();
  // set text color / Textfarbe setzen
  display.setTextColor(_LCDML_ADAFRUIT_TEXT_COLOR);  
  // set text size / Textgroesse setzen
  display.setTextSize(_LCDML_ADAFRUIT_FONT_SIZE);


  char content_text[_LCDML_DISP_cols]; 
 
  LCDMenuLib2_menu *tmp;
  // some limit values
  uint8_t i = LCDML.MENU_getScroll();
  uint8_t maxi = _LCDML_DISP_rows - _LCDML_DSIP_use_header + i;
  uint8_t n = 0;

  uint8_t n_max             = (LCDML.MENU_getChilds() >= _LCDML_DISP_rows) ? _LCDML_DISP_rows : (LCDML.MENU_getChilds());

  uint8_t scrollbar_min     = 0;
  uint8_t scrollbar_max     = LCDML.MENU_getChilds();
  uint8_t scrollbar_cur_pos = LCDML.MENU_getCursorPosAbs();
  uint8_t scroll_pos        = ((1*n_max * _LCDML_DISP_rows) / (scrollbar_max - 1) * scrollbar_cur_pos);


  n = 0;
  i = LCDML.MENU_getScroll();

  if ((tmp = LCDML.MENU_getDisplayedObj()) != NULL)
  {
          // Display a header with the parent element name
      if(_LCDML_DSIP_use_header > 0)
      {
        display.setCursor(1, _LCDML_ADAFRUIT_FONT_H * 0);
        display.fillRect(0,0,128,8, WHITE);
        display.setTextColor(BLACK);
        // only one line
        if(LCDML.MENU_getLayer() == 0)
        {
          // this text is displayed when no header is available
          display.println(F("Settings"));
        }
        else
        {
          // Display parent name
          LCDML_getContent(content_text, LCDML.MENU_getParentID());
          display.println(content_text);
        }
        display.setTextColor(WHITE);        
     }
    // loop to display lines
    do
    {
      // check if a menu element has a condition and if the condition be true
      if (tmp->checkCondition())
      {
        // check the type off a menu element
        if(tmp->checkType_menu() == true)
        {
          // display normal content
          LCDML_getContent(content_text, tmp->getID());
          display.setCursor(6, _LCDML_ADAFRUIT_FONT_H * (n+_LCDML_DSIP_use_header));
          display.println(content_text);
        }
        else
        {
          if(tmp->checkType_dynParam()) {
            tmp->callback(n);
          }
        }
        // increment some values
        i++;
        n++;
      }
    // try to go to the next sibling and check the number of displayed rows
    } while (((tmp = tmp->getSibling(1)) != NULL) && (i < maxi));
  }

  // set cursor
  display.setCursor(0, _LCDML_ADAFRUIT_FONT_H * (LCDML.MENU_getCursorPos()+_LCDML_DSIP_use_header));
  display.println(">");
  display.display();
  
}

void mFunc_back(uint8_t param)
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);

    // end function and go an layer back
    LCDML.FUNC_goBackToMenu(1);      // leave this function and go a layer back
  }
}

void mFunc_exit(uint8_t param)
{
    LCDML_UNUSED(param);
    show_menu=false;
    LCDML.OTHER_setCursorToID(0);
    spd_filter.alpha=params.rate_filter;
    alt_filter.alpha=params.alt_filter;
    Serial.println("exit menu");
}
void mFunc_store(uint8_t param)
{
    LCDML_UNUSED(param);
    show_menu=false;
    LCDML.OTHER_setCursorToID(0);
    write_params_to_eeprom(params);
    player.add_note(Note(450,50,20));
    player.add_note(Note(450,50,50));
    Serial.println("Saved to EEPROM");
}

void mFunc_load(uint8_t param){
    LCDML_UNUSED(param);
    LCDML.OTHER_setCursorToID(0);
    params = get_default_params();
    player.add_note(Note(450,50,20));
    player.add_note(Note(450,50,20));
    Serial.println("Loaded defaults");
}


void mFunc_goToRootMenu(uint8_t param)
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);

    // go to root and display menu
    LCDML.MENU_goRoot();
  }
}

void mFunc_information(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);

    // setup function
    // clear lcd
    display.clearDisplay();  
    // set text color / Textfarbe setzen
    display.setTextColor(_LCDML_ADAFRUIT_TEXT_COLOR);  
    // set text size / Textgroesse setzen
    display.setTextSize(_LCDML_ADAFRUIT_FONT_SIZE);
  
    display.setCursor(0, _LCDML_ADAFRUIT_FONT_H * 0); // line 0
    display.println(F("To close this")); 
    display.setCursor(0, _LCDML_ADAFRUIT_FONT_H * 1); // line 1
    display.println(F("function press")); 
    display.setCursor(0, _LCDML_ADAFRUIT_FONT_H * 2); // line 2
    display.println(F("any button or use")); 
    display.setCursor(0, _LCDML_ADAFRUIT_FONT_H * 3); // line 3
    display.println(F("back button")); 
    display.display(); 
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
    // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
    // the quit button works in every DISP function without any checks; it starts the loop_end function
    if(LCDML.BT_checkAny()) // check if any button is pressed (enter, up, down, left, right)
    {
      // LCDML_goToMenu stops a running menu function and goes to the menu
      LCDML.FUNC_goBackToMenu();
    }
  }

  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}

