/*------------------------------------------------------------------------------
  09/30/2019
  Author: Cisco • A C R O B O T I C 
  Platforms: ESP32
  Language: C++/Arduino
  File: simple_client.ino
  ------------------------------------------------------------------------------
  Description: 
  Simple WiFi client demo for the ESP32 — better than the library example.
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

// Initialize network parameters
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
const int http_port = 80;
const char* host = "postman-echo.com";

// Declare WiFi client variable
WiFiClient client;

void setup()
  // Connect to WiFi
  WiFi.begin(ssid,password);
  Serial.begin(115200);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("IP Address (AP): "); Serial.println(WiFi.localIP());

  // Attempt to connect to HTTP client
  if (!client.connect(host, http_port)) {
    Serial.println("Connection failed");
    return;
  } else {
    Serial.println("Connected");
    
    // Make an HTTP request:
    client.print(String("GET /get?foo1=bar1&foo2=bar2") + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n" +
                 "\r\n"
                );    
    unsigned long timeout = millis();
    while (client.available() == 0) {
      String line = client.readStringUntil('\r');
      // Serial.print(line);      
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
      }
    }
    
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }    
  }  
}

void loop()
{
}
