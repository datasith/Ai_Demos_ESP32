/*------------------------------------------------------------------------------
  02/11/2020
  Author: Cisco • A C R O B O T I C 
  Platforms: ESP32
  Language: C++/Arduino
  File: vl54l0x_oled.ino
  ------------------------------------------------------------------------------
  Description:
  Code for YouTube video tutorial demonstrating how to build a distance
  measurement device using an ESP32 connected to a VL53L0X laser-ranging module.
  In addition, the ESP32 is connected to an OLED screen so that the distance
  measurements can be visualized without the need of a computer:
  https://youtu.be/gpx7Qu6c4IE
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
// Include the necessary libraries
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>

// Instantiate an object for the OLED screen
U8G2_SSD1306_64X48_ER_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Instantiate an object for the sensor
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  // Initialize Serial communication for debugging
  Serial.begin(115200);
  Serial.println("VL53L0X Test");
  // Initialize the screen
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  // Initialize the sensor
  if(!lox.begin()) {
    Serial.println("Failed to initialize VL53L0X");
    while(1);
  }
}

void loop() {
  // Declare variables for storing the sensor data
  VL53L0X_RangingMeasurementData_t value;
  String output;
  // Get the sensor data
  Serial.print("Reading a measurement...");
  lox.rangingTest(&value, false);
  // If we get a valid measurement, send it to the screen
  if(value.RangeStatus != 4) {
    output = String(value.RangeMilliMeter) + " mm";
    Serial.print("Distance: ");
    Serial.println(output);
    // Make sure the screen is clear
    u8g2.clearBuffer();
    // Display a header
    u8g2.drawStr(0,10,"Distance:");
    // Display the data
    u8g2.drawStr(0,20,output.c_str());
    u8g2.sendBuffer();
  } else {
    Serial.println(" Out of range! ");
  }
  // Wait a bit before the next measurement
  delay(100);
}
