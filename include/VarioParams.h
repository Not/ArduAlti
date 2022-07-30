
#ifndef VARIOPARAMS_H
#define VARIOPARAMS_H

#include <Arduino.h>

struct VarioParams{
  float a_rate;
  u_int32_t a_pitch;

  float b_rate;
  u_int32_t b_pitch;
  
  float c_rate;
  u_int32_t c_pitch;
  u_int32_t c_period;

  float d_rate;
  u_int32_t d_pitch;
  u_int32_t d_period;

  float min_rate_lcd;
  float max_rate_lcd;
};

void print_params(VarioParams p){
  Serial.println("VarioParams:");
  Serial.print("\tc_rate ");Serial.println(p.c_rate);
  Serial.print("\td_rate ");Serial.println(p.d_rate);
  Serial.print("\tc_pitch ");Serial.println(p.c_pitch);
  Serial.print("\td_pitch ");Serial.println(p.d_pitch);  
}
#endif

