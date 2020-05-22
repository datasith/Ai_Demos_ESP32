/*------------------------------------------------------------------------------
  09/30/2019
  Author: Cisco • A C R O B O T I C 
  Platforms: ESP32
  Language: C++/Arduino
  File: compass_visualization.ino
  ------------------------------------------------------------------------------
  Description: 
  Code for YouTube video demonstrating how to visualize sensor data for a 3-axis 
  I2C magnetometer (QMC5833/HMC5833) using the JavaScript library Babylon JS 
  which allows us to render HTML5 3d objects on a webpage.
  https://youtu.be/kjw_lV1-wY8
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
// Update the respective headers for the ESP32
#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <WebSocketsServer.h>

#include <Wire.h>
#include <QMC5883L.h>

WebServer server;
char* ssid = "YOUR_SSID";
char* password = "YOUR_PASSWORD";

// hold uploaded file
File fsUploadFile;

// websockets server for handling messages sent by the client
WebSocketsServer webSocket = WebSocketsServer(81);

// declare a variable to hold the sensor object
QMC5883L compass;

void setup()
{
  // Format the file system in case it hasn't been done before
  SPIFFS.begin(true);
  WiFi.begin(ssid,password);
  Serial.begin(115200);
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/",handleIndexFile);
  // list available files
  server.on("/list", HTTP_GET, handleFileList);
  // handle file upload
  server.on("/upload", HTTP_POST, [](){
    server.send(200, "text/plain", "{\"success\":1}");
  }, handleFileUpload);
  server.begin();
  webSocket.begin();

  Wire.begin();
  compass.init();
  compass.setSamplingRate(50);
}

void loop()
{
  webSocket.loop();
  server.handleClient();
  // collect and print the data every ~20ms
  static unsigned long l = 0;
  unsigned long t = millis();
  if((t - l) > 20) {
//    Serial.println(compass.readHeading());
    // send the heading angle to the websocket client(s) as a JSON-formatted string
    String output = "{";
    output += "\"heading\":";
    output += compass.readHeading();
    output += "}";
    webSocket.broadcastTXT(output);
    l = t;
  }
}

// Update file handling functions for the ESP32
void handleFileUpload() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) {
      filename = "/" + filename;
    }
    Serial.print("handleFileUpload Name: "); Serial.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    //Serial.print("handleFileUpload Data: "); Serial.println(upload.currentSize);
    if (fsUploadFile) {
      fsUploadFile.write(upload.buf, upload.currentSize);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) {
      fsUploadFile.close();
    }
    Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
  }
}

void handleIndexFile()
{
  File file = SPIFFS.open("/index.html","r");
  server.streamFile(file, "text/html");
  file.close();
}

void handleFileList() {
  if (!server.hasArg("dir")) {
    server.send(500, "text/plain", "BAD ARGS");
    return;
  }

  String path = server.arg("dir");
  Serial.println("handleFileList: " + path);


  File root = SPIFFS.open(path);
  path = String();

  String output = "[";
  if(root.isDirectory()){
      File file = root.openNextFile();
      while(file){
          if (output != "[") {
            output += ',';
          }
          output += "{\"type\":\"";
          output += (file.isDirectory()) ? "dir" : "file";
          output += "\",\"name\":\"";
          output += String(file.name()).substring(1);
          output += "\"}";
          file = root.openNextFile();
      }
  }
  output += "]";
  server.send(200, "text/json", output);
}

bool exists(String path){
  bool yes = false;
  File file = SPIFFS.open(path, "r");
  if(!file.isDirectory()){
    yes = true;
  }
  file.close();
  return yes;
}
