/*------------------------------------------------------------------------------
  05/21/2020
  Authors: Cisco • A C R O B O T I C | Edrig
  Platforms: ESP32
  Language: C++/Arduino
  File: twitch_chat_ws2812b.ino
  ------------------------------------------------------------------------------
  Description: 
  Code for YouTube video tutorial demonstrating how to use a websocket client to
  get messages from Twitch Chat. The code uses the messages—formatted in a
  specific way—to control 16 NeoPixel/WS2812B LEDs: 
  https://youtu.be/xHSg7IpmLo4
  You can also try out the code live:
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
#define N_PIXELS  16
#define LED_PIN   19       // labeled "D6" on the board
#define LED_TYPE  WS2811
#define BRIGHTNESS  24     // a little dim for recording purposes
#define COLOR_ORDER GRB

// declare the LED array
CRGB leds[N_PIXELS];

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
        String pixel_str = payload_str.substring(quote_start+2, quote_end);
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
  if(doc.containsKey("led")) {
    String val = doc["led"];
    uint8_t i, r, g, b;
    uint8_t result = sscanf(val.c_str(), "%d,%d,%d,%d", &i, &r, &g, &b);
    setRgb(i, r, g, b);
  }
}

void setRgb(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
  if(i < N_PIXELS) {
    leds[i].r = r;
    leds[i].g = g;
    leds[i].b = b;
    FastLED.show();
  }
}

void loop() {
  webSocket.loop();
}
