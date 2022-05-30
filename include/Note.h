
#ifndef NOTE_H
#define NOTE_H

#include <Arduino.h>

struct Note{
  Note(){}
  Note(uint32_t frequency_, uint32_t duration_, uint32_t pause_after_,uint8_t volume_=255):
  frequency{frequency_}, duration{duration_}, pause_after{pause_after_}, volume{volume_}{}
  uint32_t frequency;
  uint32_t duration;
  uint32_t pause_after;
  uint8_t volume;
  void print(Stream *serial){
    char buf[64];
    sprintf(buf, "Note(%d Hz, %d/%d ms)", frequency, duration, pause_after);
    serial->println(buf);
  }
};
#endif