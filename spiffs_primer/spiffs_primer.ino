/*------------------------------------------------------------------------------
  08/10/2020
  Author: Cisco • A C R O B O T I C 
  Platforms: ESP32
  Language: C++/Arduino
  File: spiffs_primer.ino
  ------------------------------------------------------------------------------
  Description: 
  Code for YouTube video demonstrating how to get started with reading and
  writing JSON-formatted data to flash memory (SPIFFS):
  https://youtu.be/
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
#include <SPIFFS.h>
#include <ArduinoJson.h>

void setup() {
  Serial.begin(115200);
  // Initialize the SPIFFS object
  if(!SPIFFS.begin(true)) {
    Serial.println("Error initializing SPIFFS");
    while(true){}
  }

  const char* filename = "/data.json";
  writeDataToFile(filename);
  listAllFiles();  
  readDataFromFile(filename);
}

void loop() {}

void listAllFiles() {
  // List all available files (if any) in the SPI Flash File System
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while(file) {
    Serial.print("FILE: ");
    Serial.println(file.name());
    file = root.openNextFile();
  }
  root.close();
  file.close();  
}

void readDataFromFile(const char* filename) {
  // Read JSON data from a file
  File file = SPIFFS.open(filename);
  if(file) {
    // Deserialize the JSON data
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, file);
    double data_str = doc["data_out"];
    Serial.println(data_str);
  }
  file.close();  
}

void writeDataToFile(const char* filename) {
  File outfile = SPIFFS.open(filename,"w");
  StaticJsonDocument<200> doc;
  doc["data_out"] = 3.14159;
  if(serializeJson(doc, outfile)==0) {
    Serial.println("Failed to write to SPIFFS file");
  } else {
    Serial.println("Success!");
  }
  outfile.close();  
}
