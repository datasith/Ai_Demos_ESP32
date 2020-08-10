#ifndef _PIXELATE_H_
#define _PIXELATE_H_
#include <Arduino.h>
#include <FastLED.h>

class Pixelate
{
  public:
    void begin();
    void setRgb();
};

extern Pixelate Pixelator;

#endif
