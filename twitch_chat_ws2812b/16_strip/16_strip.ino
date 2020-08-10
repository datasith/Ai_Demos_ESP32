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
#include <FastLED.h>
#include "secret.h"
#include "util.h"
#include "pixelate.h"

// Declare websocket client class variable
WebSocketsClient webSocket;

// Declared message handler class variable
MsgHandler msgHandler(&webSocket);

void setup() {
  // Connect to WiFi
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  Serial.begin(115200);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("IP Address: "); Serial.println(WiFi.localIP());
  
  // Websocket event handler
  webSocket.onEvent(webSocketEvent);

  // Websocket communication handler
  msgHandler.begin();
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch(type) {
    // If the websocket connection is succesful, try to join the IRC server
    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to: %s\n", payload);
      msgHandler.connect();
      break;
    // If we get a message, print it to Serial and attempt to parse it
    case WStype_TEXT: {
      Serial.printf("> %s\n", payload);
      msgHandler.parse(payload);
      break;
    }
    // Handle disconnecting from the websocket
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      msgHandler.connect();      
      break;
  }
}

void loop() {
  webSocket.loop();
}
