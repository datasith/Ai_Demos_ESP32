/*------------------------------------------------------------------------------
  08/01/2020
  Authors: Cisco • A C R O B O T I C
  Platforms: ESP32
  Language: C++/Arduino
  File: barebones.ino
  ------------------------------------------------------------------------------
  Description: 
  Boiler plate code for starting a project with the ESP32.
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
const char ssid[] = "YOUR_SSID"; 
const char password[] = "YOUR_PASSWORD";

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
}

void loop() {

}
