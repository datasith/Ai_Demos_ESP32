/*------------------------------------------------------------------------------
  09/30/2019
  Author: Cisco • A C R O B O T I C 
  Platforms: ESP32
  Language: C++/Arduino
  File: simple_fastled.ino
  ------------------------------------------------------------------------------
  Description: 
  Simple FastLED demo for the ESP32 with options to work right off the bat.
  ------------------------------------------------------------------------------
  Do you like my work? You can support me:
  https://patreon.com/acrobotic
  https://paypal.me/acrobotic
  https://buymeacoff.ee/acrobotic
  ------------------------------------------------------------------------------
  Please consider buying products and kits to help fund future Open-Source 
  projects like this! We'll always put our best effort in every project, and 
  release all our design files and code for you to use. 
  https://acrobotic.com/
  https://amazon.com/shops/acrobotic
  ------------------------------------------------------------------------------
  License:
  Please see attached LICENSE.txt file for details.
  ------------------------------------------------------------------------------*/
#include<FastLED.h>

// define necessary parameters
#define N_PIXELS  16
#define LED_PIN   19 // labeled "D6" on the board
#define LED_TYPE  WS2811
#define BRIGHTNESS  24     // a little dim for recording purposes
#define COLOR_ORDER GRB

// declare the LED array
CRGB leds[N_PIXELS];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  // initialize the LED object
  FastLED.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(leds,N_PIXELS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  for(uint8_t i = 0; i < N_PIXELS; i++) {
    leds[i] = Wheel( map( i, 0, N_PIXELS-1, 30, 150 ) );
  }
  FastLED.show();
}

CRGB Wheel(byte WheelPos) {
  // return a color value based on an input value between 0 and 255
  if(WheelPos < 85)
    return CRGB(WheelPos * 3, 255 - WheelPos * 3, 0);
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return CRGB(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return CRGB(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
