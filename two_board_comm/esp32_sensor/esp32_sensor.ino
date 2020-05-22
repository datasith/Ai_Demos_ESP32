/*------------------------------------------------------------------------------
  05/10/2020
  Author: Cisco • A C R O B O T I C 
  Platforms: ESP32
  Language: C++/Arduino
  File: File: esp32_sensor.ino
  ------------------------------------------------------------------------------
  Description: 
  Code for YouTube video tutorial demonstrating how to send and receive data 
  between to ESP32 in real-time. This file sets up a Websocket server to gather 
  data from a BME680 that's connected over I2C. The data is formatted in JSON 
  and broadcasted on the Websocket:
  https://youtu.be/tGR5zqN9M2E  
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
#include <WebSocketsServer.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <Ticker.h>

// Initialize network parameters
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Instantiate server objects
WebServer server;
WebSocketsServer webSocket = WebSocketsServer(81);

// Declare/initialize timer variables
Ticker timer;
bool read_data = false;

// Declare sensor variables
Adafruit_BME680 bme; // I2C

// Raw string literal for containing the page to be sent to clients
char webpage[] PROGMEM = R"=====(
<html>
<head>
  <script>
    var Socket;
    function init() {
      Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
      Socket.onmessage = function(event){
        // receive the color data from the server
        var data = JSON.parse(event.data);
        console.log(data);
      }
    }  
  </script>
</head>
<body onload="javascript:init()">
  <h4>Websocket client served from the Sensor Board!</h4>
</body>
</html>
)=====";

void setup() {
  // Connect to WiFi
  WiFi.begin(ssid,password);
  Serial.begin(115200);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("IP Address (AP): "); Serial.println(WiFi.localIP());

  // define the routes in which the server is accessible
  server.on("/",[](){
    server.send_P(200, "text/html", webpage);  
  });

  // initialize server and websockets
  server.begin();
  webSocket.begin();
  
  // initialize timer function
  timer.attach(/*rate in secs*/ 0.1, /*callback*/ readData);
  
  // initialize and configure the BME680 sensor
  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  // handling incoming messages on the websocket
  webSocket.onEvent(webSocketEvent);
}

void readData() {
  // should only be used to set/unset flags
  read_data = true;
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  if(type == WStype_TEXT)
  {
    // processs any returned data
    Serial.printf("payload [%u]: %s\n", num, payload);
  }
}

void loop() {
  webSocket.loop();
  server.handleClient();  
  if(read_data){
    if (! bme.performReading()) {
      Serial.println("Failed to perform reading :(");
      return;
    }
    String json = "{\"temperature\":";
    json += bme.temperature;
    json += ",\"humidity\":";
    json += bme.humidity;
    json += ",\"pressure\":";
    json += bme.pressure;
    json += ",\"gas\":";
    json += bme.gas_resistance / 1000.0;
    json += "}";
//    Serial.println(json); // DEBUGGING
    webSocket.broadcastTXT(json.c_str(), json.length());
  }
}
