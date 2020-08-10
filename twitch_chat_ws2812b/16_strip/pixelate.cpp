#include "pixelate.h"

// Define necessary parameters for controlling the WS2812B LEDs
#define N_PIXELS    16
#define LED_PIN     19     // labeled "D6" on the board
#define LED_TYPE    WS2811
#define BRIGHTNESS  24     // a little dim for recording purposes
#define COLOR_ORDER GRB

// declare the LED array
CRGB leds[N_PIXELS];

void Pixelate::begin() {
  // initialize the FastLED object
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, N_PIXELS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear(true);  
}

void Pixelate::setRgb() {
  uint8_t i = 0;
  if(i < N_PIXELS) {
    leds[i].r = 255;
    leds[i].g = 255;
    leds[i].b = 0;
    FastLED.show();
  }  
}

class Pixelate Pixelator;
