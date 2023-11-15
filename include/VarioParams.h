
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

#endif

