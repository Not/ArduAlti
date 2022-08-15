#ifndef CHARGE_STATE_H
#define CHARGE_STATE_H
#include <Arduino.h>
#include <Globals.h>
class ChargeState{
  public:
    int battery_voltage_ms;
    int batter_percentage;
    int battery_voltage_curve[20] = VOLTAGES_LEVELS;  //can I get rid of this 20?
    bool charging;

    void set_from_measured_voltage(int voltage){
      if(voltage>4500){
        battery_voltage_ms = voltage;
        charging=true;
      }
      else{
        battery_voltage_ms = voltage + DIODE_VOLTAGE_DROP;
        batter_percentage = get_percentage_from_voltage(battery_voltage_ms);
        charging=false;
      }
    }
    void print(Stream *serial){
      char buf[64];
      sprintf(buf, "(%d mV, 0.%d,  %scharging)", battery_voltage_ms, batter_percentage, charging?"":"dis");
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
