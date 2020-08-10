/*------------------------------------------------------------------------------
  05/21/2020
  Authors: Cisco • A C R O B O T I C | Edrig
  Platforms: ESP32
  Language: C++/Arduino
  File: 8x8_matrix.ino
  ------------------------------------------------------------------------------
  Description: 
  Same sketch as twitch_chat_ws2812b.ino for controlling an 8x8-pixel matrix of 
  NeoPixel/WS2812B LEDs using Twitch Chat. Adding features for running the code 
  live at:
  https://www.twitch.tv/acrobotik
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
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <FastLED.h>
#include "secret.h"
#include "util.h"

// Initialize network parameters
const char ssid[] = SECRET_SSID; 
const char password[] = SECRET_PASS;

// Declare websocket client class variable
WebSocketsClient webSocket;

// Allocate the JSON document
StaticJsonDocument<200> doc;

// Parameters for Twitch channel
const char twitch_oauth_token[] = TWITCH_OAUTH_TOKEN;
const char twitch_nick[] = TWITCH_NICK;    
const char twitch_channel[] = TWITCH_CHANNEL;

// Define necessary parameters for controlling the WS2812B LEDs
#define N_PIXELS    64
#define LED_PIN     19     // labeled "D6" on the board
#define LED_TYPE    WS2811
#define BRIGHTNESS  12     // a little dim for recording purposes
#define COLOR_ORDER GRB

// declare the LED array
CRGB leds[N_PIXELS];

CRGBPalette16 currentPalette( HeatColors_p );
CRGBPalette16 targetPalette( PartyColors_p );

// Map the serpentile-style matrix to the NeoPixel format
const uint8_t remap[8][8] = {
  {7, 6, 5, 4, 3, 2, 1, 0},
  {8, 9, 10, 11, 12, 13, 14, 15},
  {23, 22, 21, 20, 19, 18, 17, 16},
  {24, 25, 26, 27, 28, 29, 30, 31},
  {39, 38, 37, 36, 35, 34, 33, 32},
  {40, 41, 42, 43, 44, 45, 46, 47},
  {55, 54, 53, 52, 51, 50, 49, 48},
  {56, 57, 58, 59, 60, 61, 62, 63},
};

// Gamma correction table
const uint8_t PROGMEM gamma8[] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
  10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
  17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
  25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
  37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
  51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
  69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
  90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
  115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
  144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
  177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
  215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
};

void setup() {
  // Connect to WiFi
  WiFi.begin(ssid,password);
  Serial.begin(115200);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("IP Address: "); Serial.println(WiFi.localIP());

  // server address, port, and URL path
  webSocket.begin("irc-ws.chat.twitch.tv", 80, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);

  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);

  // initialize the FastLED object
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, N_PIXELS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear(true);
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    // If the websocket connection is succesful, try to join the IRC server
    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to: %s\n", payload);
      webSocket.sendTXT("PASS " + String(twitch_oauth_token) + "\r\n");
      webSocket.sendTXT("NICK " + String(twitch_nick) + "\r\n");
      webSocket.sendTXT("JOIN " + String(twitch_channel) + "\r\n");
      break;
    // If we get a response, print it to Serial
    case WStype_TEXT: {
      Serial.printf("> %s\n", payload);
      String payload_str = String((char*) payload);
      // Search for the beginning on the JSON-encoded message (":!")
      int quote_start = payload_str.indexOf(":!");
      // If the message is addressed to the chat bot
      if(quote_start > 0) {
        int quote_end = payload_str.length();
        String pixel_str = payload_str.substring(quote_start+2, quote_end-2);
        pixel_str.toLowerCase();
        // might need abstraction (functionality needed everywhere?)
        if(pixel_str=="help") {
          sendMessage(MSG_HELP);
          return;
        }
        // really, really half-baked implementation
        if(pixel_str=="rainbow") {
          Serial.println();
          animate(); // change to non-blocking
        }
        Serial.println(pixel_str);
        parseMessage(pixel_str);
      }
      break;
    }
    // Handle disconnecting from the websocket
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      webSocket.sendTXT("PART " + String(TWITCH_CHANNEL) + "\r\n");
      break;
  }
}

void animate() {
  EVERY_N_MILLISECONDS( 60 ) {
    pallet_fade();
  }
}

void ChangePalettePeriodically(){
  uint8_t secondHand = (millis() / 1000) % 60;
  static uint8_t lastSecond = 99;
  
  if( lastSecond != secondHand ) {
    lastSecond = secondHand;
    CRGB p = CHSV( HUE_PURPLE, 255, 255);
    CRGB g = CHSV( HUE_GREEN, 255, 255);
    CRGB b = CRGB::Black;
    CRGB w = CRGB::White;
    if( secondHand ==  0)  { targetPalette = RainbowColors_p; }
    if( secondHand == 10)  { targetPalette = CRGBPalette16( g,g,b,b, p,p,b,b, g,g,b,b, p,p,b,b); }
    if( secondHand == 20)  { targetPalette = CRGBPalette16( b,b,b,w, b,b,b,w, b,b,b,w, b,b,b,w); }
    if( secondHand == 30)  { targetPalette = LavaColors_p; }
    if( secondHand == 40)  { targetPalette = CloudColors_p; }
    if( secondHand == 50)  { targetPalette = PartyColors_p; }
  }
}

void FillLEDsFromPaletteColors( uint8_t colorIndex){
  
  for( int i = 0; i < N_PIXELS; i++) {
    leds[i] = ColorFromPalette( currentPalette, colorIndex + sin8(i*16), BRIGHTNESS);
    colorIndex += 3;
  }
}

//from https://gist.github.com/kriegsman/1f7ccbbfa492a73c015e
void pallet_fade(){
  
  ChangePalettePeriodically();

  // Crossfade current palette slowly toward the target palette
  //
  // Each time that nblendPaletteTowardPalette is called, small changes
  // are made to currentPalette to bring it closer to matching targetPalette.
  // You can control how many changes are made in each call:
  //   - the default of 24 is a good balance
  //   - meaningful values are 1-48.  1=veeeeeeeery slow, 48=quickest
  //   - "0" means do not change the currentPalette at all; freeze
  
  uint8_t maxChanges = 24; 
  nblendPaletteTowardPalette( currentPalette, targetPalette, maxChanges);


  static uint8_t startIndex = 0;
  startIndex = startIndex + 1; /* motion speed */
  FillLEDsFromPaletteColors( startIndex);

  FastLED.show();
}

void sendMessage(String msg) {
  String output = "PRIVMSG "+String(twitch_channel);
  output += " :";
  output += msg;
  Serial.println(output);
  webSocket.sendTXT(output);
  return;
}

void parseMessage(String pixel_str) {
  // Attempt to deserialize the string
  DeserializationError error = deserializeJson(doc, pixel_str);
  // Test to see if the string is a valid JSON-formatted one
  if(error) {
    Serial.print("deserializeJson failed: ");
    Serial.println(error.c_str());
    return;
  }
  // Assume that a valid string is of the form {"led":"11,255,255,255"}
  // for turning pixel #12 to full white
  if(doc.containsKey("matrix")) {
    String val = doc["matrix"];
    uint8_t row, col, r, g, b;
    uint8_t result = sscanf(val.c_str(), "%d,%d,%d,%d,%d", &row, &col, &r, &g, &b);
    setRgb(row, col, r, g, b);
  }
}

void setRgb(uint8_t row, uint8_t col, uint8_t r, uint8_t g, uint8_t b) {
  uint8_t i = remap[row][col];
  if(i < N_PIXELS) {
    leds[i].r = pgm_read_byte(&gamma8[r]);
    leds[i].g = pgm_read_byte(&gamma8[g]);
    leds[i].b = pgm_read_byte(&gamma8[b]);
    FastLED.show();
  }
}

void loop() {
  webSocket.loop();
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
