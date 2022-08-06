

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