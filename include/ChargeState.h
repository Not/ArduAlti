#ifndef CHARGE_STATE_H
#define CHARGE_STATE_H
#define BAT_HIGH_CHARGE 22  // HIGH for 50mA, LOW for 100mA
#define BAT_CHARGE_STATE 23 // LOW for charging, HIGH not charging
#define VBAT_MV_PER_LBS (0.003395996F)
#include <Arduino.h>
#include <Globals.h>
class ChargeState{
  public:
    int battery_voltage_mV;
    int batter_percentage;
    int battery_voltage_curve[20] = VOLTAGES_LEVELS;  //can I get rid of this 20?
    bool charging;
    void begin(){
      pinMode(VBAT_ENABLE, OUTPUT);
      pinMode(BAT_CHARGE_STATE, INPUT);

      digitalWrite(BAT_HIGH_CHARGE, LOW); // charge with 100 mA
      set_from_readings();
    }
    
    void set_from_readings(){
      digitalWrite(VBAT_ENABLE, LOW);
      uint32_t adcCount = analogRead(PIN_VBAT);
      float adcVoltage = adcCount * VBAT_MV_PER_LBS;
      battery_voltage_mV = adcVoltage * (1510.0 / 510.0) * 1000;
      charging = digitalRead(BAT_CHARGE_STATE) == LOW;
      digitalWrite(VBAT_ENABLE, HIGH);
      batter_percentage = get_percentage_from_voltage(battery_voltage_mV);
    }
    void print(Stream *serial){
      char buf[64];
      sprintf(buf, "(%d mV, 0.%d,  %scharging)", battery_voltage_mV, batter_percentage, charging?"":"dis");
      serial->println(buf);
    }
    
  private:
      int get_percentage_from_voltage(int battery_voltage_ms ){
      int voltage_steps = sizeof(battery_voltage_curve)/sizeof(battery_voltage_curve[0]);
      for(int i=0; i<voltage_steps;i++){
        if (battery_voltage_ms > battery_voltage_curve[i]){
          return 100-i*(100/voltage_steps);
        }
      }
      return 0;
    }
};
#endif
