/*------------------------------------------------------------------------------
  04/11/2020
  Author: Makerbro
  Platforms: ESP32
  Language: C++/Arduino
  File: vl54l0x_oled.ino
  ------------------------------------------------------------------------------
  Description:
  Code for YouTube video tutorial demonstrating how to build a temperature
  measurement device using an ESP32 connected to a MLX90614 digital non-contact
  infrared thermometer. In addition, the ESP32 is connected to an OLED screen so 
  that the temperature measurements can be visualized without the need of a 
  computer. Lastly, the code includes a web server to provide additional access
  to the measurements:
  https://youtu.be/1Gvv6OX0ZoE

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
// Include the necessary libraries
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>

// Instantiate an object for the OLED screen
U8G2_SSD1306_64X48_ER_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Instantiate an object for the sensor
// SCL -> GPIO22 (D1 on a D1 Mini)
// SDA -> GPIO21 (D2 on a D1 Mini)
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup() {
  // Initialize Serial communication for debugging
  Serial.begin(115200);
  Serial.println("MLX90614 Test");

  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  // Initialize the screen
  u8g2.setBusClock(100000);
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  // Initialize the sensor
  if(!mlx.begin()) {
    Serial.println("Failed to initialize MLX90614");
    while(1);
  }
}

void loop() {
  // Declare variables for storing the sensor data
  double temp_obj, temp_amb;
  // Get the sensor data
  Serial.print("Reading a measurement...");
  temp_amb = mlx.readAmbientTempC(); //temperature of the sensor itself
  temp_obj = mlx.readObjectTempC();  //temperature of what it's measuring in the 90-degree field of view
  Serial.print("Ambient = "); 
  Serial.print(temp_amb); 
  Serial.print("*C\tObject = "); 
  Serial.print(temp_obj); Serial.println("*C");

  String output = String(temp_amb) + " *C";
  // Make sure the screen is clear
  u8g2.clearBuffer();
  // Display a header
  u8g2.drawStr(0,10,"Ambient:");
  // Display the data
  u8g2.drawStr(0,20,output.c_str());
  // Display a header
  u8g2.drawStr(0,30,"Object:");
  output = String(temp_obj) + " *C";  
  // Display the data
  u8g2.drawStr(0,40,output.c_str());  
  u8g2.sendBuffer();

  // Wait a bit before the next measurement
  delay(500);
}
