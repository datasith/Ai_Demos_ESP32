/*------------------------------------------------------------------------------
  05/10/2020
  Author: Makerbro
  Platforms: ESP32
  Language: C++/Arduino
  File: esp32_oled.ino
  ------------------------------------------------------------------------------
  Description: 
  Code for YouTube video tutorial demonstrating how to send and receive data 
  between to ESP32 in real-time. This file sets up a Websocket client to receive
  data from a sensor and display it on an OLED screen:
  https://youtu.be/tGR5zqN9M2E
  
  Do you like my videos? You can support the channel:
  https://patreon.com/acrobotic
  https://paypal.me/acrobotic
  ------------------------------------------------------------------------------
  Please consider buying products from ACROBOTIC to help fund future
  Open-Source projects like this! We'll always put our best effort in every
  project, and release all our design files and code for you to use. 

  https://acrobotic.com/
  https://amazon.com/shops/acrobotic
  ------------------------------------------------------------------------------
  License:
  Please see attached LICENSE.txt file for details.
------------------------------------------------------------------------------*/
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsClient.h>
#include <U8g2lib.h>
#include <ArduinoJson.h>

// Initialize sensor parameters
float temperature = 0.0, humidity = 0.0, pressure = 0.0, gas = 0.0;

// Instantiate an object for the OLED screen
U8G2_SSD1306_64X48_ER_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Initialize network parameters
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Instantiate WiFi/Websocket client objects and parameters
WebSocketsClient webSocket;

// Allocate the JSON document
StaticJsonDocument<200> doc;

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

  // Initialize the screen
  u8g2.setBusClock(100000);
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr);

  // server address, port, and URL path
  webSocket.begin("SENSOR_BOARD_IP", 81, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);

  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  // Make sure the screen is clear
  u8g2.clearBuffer();
  if(type == WStype_TEXT)
  {
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, payload);
  
    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }
    const float t = doc["temperature"];
    const float h = doc["humidity"];
    const float p = doc["pressure"];
    const float g = doc["gas"];
    
    // Display the Data
    u8g2.drawStr(0,10,"T:");
    u8g2.drawStr(12,10,String(t).c_str());
    u8g2.drawStr(0,22,"H:");
    u8g2.drawStr(12,22,String(h).c_str());
    u8g2.drawStr(0,34,"P:");
    u8g2.drawStr(12,34,String(p).c_str());
    u8g2.drawStr(0,46,"G:");
    u8g2.drawStr(12,46,String(g).c_str());

    // Send acknowledgement to the client
    webSocket.sendTXT("{\"status\":\"OK\"}");
  }
  u8g2.sendBuffer();
}

void loop() {
  webSocket.loop();
}
