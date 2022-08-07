
#ifndef VARIOPARAMS_H
#define VARIOPARAMS_H

#include <Arduino.h>
struct CurvePoint{
  float rate;
  u_int32_t pitch;
  u_int32_t period;
};

struct VarioParams{
  CurvePoint a;
  CurvePoint b;
  CurvePoint c;
  CurvePoint d;
  float min_rate_lcd;
  float max_rate_lcd;
  float rate_filter;
  float alt_filter;
};

// void print_params(VarioParams p){
//   Serial.println("VarioParams:");
//   Serial.print("\tc_rate ");Serial.println(p.c_rate);
//   Serial.print("\td_rate ");Serial.println(p.d_rate);
//   Serial.print("\tc_pitch ");Serial.println(p.c_pitch);
//   Serial.print("\td_pitch ");Serial.println(p.d_pitch);  
// }
#endif

