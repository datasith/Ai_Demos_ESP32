#include "util.h"

const char* twitch_oauth_token = TWITCH_OAUTH_TOKEN;
const char* twitch_nick = TWITCH_NICK;
const char* twitch_channel = TWITCH_CHANNEL;

#define NUM_HELP 4
#define NUM_CONNECT 3
#define NUM_DISCONNECT 3
#define NUM_ERROR 1

const char *msg_help[NUM_HELP] = {"[1/4] copy/type including the exclamation point: !device:X,Y,R,G,B", \
            "[2/4] change 'device' to stick, matrix, ring, ardushield", \
            "[3/4] change 'X,Y' to numbers between 0~15 (X is row, Y is column)", \
            "[4/4] change 'R,G,B' to numbers between 0~255 (R is Red, G is Green, B is Blue)."};
const char *msg_error[NUM_ERROR] = {"no-no, try again."};
// Using String instead of char for easiness/laziness of concatenation
const String msg_connect[NUM_CONNECT] = {
  "PASS " + String(twitch_oauth_token) + "\r\n", \
  "NICK " + String(twitch_nick) + "\r\n", \
  "JOIN " + String(twitch_channel) + "\r\n"};
const String msg_disconnect[NUM_DISCONNECT] = {
  "PART " + String(twitch_channel) + "\r\n"};

struct {
  char *help = "help";
  char *stick = "stick";  
} commands;

struct {
  uint8_t row;
  uint8_t col;
  uint8_t r;
  uint8_t g;
  uint8_t b;
} pixeldata;

MsgHandler::MsgHandler(WebSocketsClient *webSocket) {
  _webSocket = webSocket;
}

bool MsgHandler::begin() {
  // server address, port, and URL path
  _webSocket->begin("irc-ws.chat.twitch.tv", 80, "/");
  // try ever 5000 again if connection has failed
  _webSocket->setReconnectInterval(5000);
  // Initialize the LED pixel controller
  Pixelator.begin();  
  return true;
}

bool MsgHandler::connect() {
  sendTxt(msg_connect, NUM_CONNECT);
  return true;
}

bool MsgHandler::disconnect() {
  sendTxt(msg_disconnect, NUM_CONNECT);
  return true;
}

bool MsgHandler::parse(uint8_t *payload) {
  String payload_str = String((char*) payload);
  // Search for the beginning on the JSON-encoded message (":!")
  int msg_start = payload_str.indexOf(":!");
  // If the message is addressed to the chat bot
  if(msg_start > 0) {
    int msg_end = payload_str.length();
    String msg_str = payload_str.substring(msg_start+2, msg_end);
    Serial.print("Processing... ");
    Serial.println(msg_str);
    // Start the parsing
    // ===================
    // parameter 1: device or operation
    String param_1;
    int idx = msg_str.indexOf(',');
    if(idx>0)
      param_1 = msg_str.substring(0,idx);
    else
      param_1 = msg_str.substring(0,msg_str.length()-2);
    if(strcmp(commands.help,param_1.c_str())==0) {
      sendTxt(msg_help, NUM_HELP);
    } else if(strcmp(commands.stick,param_1.c_str())==0) {
      msg_str = msg_str.substring(idx+1,msg_str.length());
      if( getPixelData(msg_str) )
//        Serial.println("Set Pixel Color");
        Pixelator.setRgb();
      else
        sendTxt(msg_error, NUM_ERROR);
    }
    
  }  
  return true;
}

bool MsgHandler::getPixelData(String msg_str){
  uint8_t row, col, r, g, b;  
  int idx = msg_str.indexOf(',');
  if(idx>0)
    row = msg_str.substring(0,idx).toInt();
  else
    return false;
  msg_str = msg_str.substring(idx+1,msg_str.length());
  idx = msg_str.indexOf(',');
  if(idx>0)
    col = msg_str.substring(0,idx).toInt();
  else
    return false;    
  msg_str = msg_str.substring(idx+1,msg_str.length());
  idx = msg_str.indexOf(',');
  if(idx>0)
    r = msg_str.substring(0,idx).toInt();
  else
    return false;    
  msg_str = msg_str.substring(idx+1,msg_str.length());
  idx = msg_str.indexOf(',');
  if(idx>0)
    g = msg_str.substring(0,idx).toInt();
  else
    return false;    
  msg_str = msg_str.substring(idx+1,msg_str.length());
  b = msg_str.toInt();
  return true;  
}

void MsgHandler::sendTxt(const char *msg[], uint8_t n) {
  for(uint8_t i=0;i<n;i++) {
    String output = "PRIVMSG "+String(twitch_channel);
    output += " :";
    output += msg[i];  
    _webSocket->sendTXT(output);
  }
}

void MsgHandler::sendTxt(const String msg[], uint8_t n) {
  for(uint8_t i=0;i<n;i++) { 
    _webSocket->sendTXT(msg[i].c_str());
  }
}

//
//void parseMessage(String pixel_str) {
//  // Attempt to deserialize the string
//  DeserializationError error = deserializeJson(doc, pixel_str);
//  // Test to see if the string is a valid JSON-formatted one
//  if(error) {
//    Serial.print("deserializeJson failed: ");
//    Serial.println(error.c_str());
//    return;
//  }
//  // Assume that a valid string is of the form {"led":"11,255,255,255"}
//  // for turning pixel #12 to full white
//  if(doc.containsKey("led")) {
//    String val = doc["led"];
//    uint8_t i, r, g, b;
//    uint8_t result = sscanf(val.c_str(), "%d,%d,%d,%d", &i, &r, &g, &b);
//    setRgb(i, r, g, b);
//  }
//}
//
//void setRgb(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
//  if(i < N_PIXELS) {
//    leds[i].r = r;
//    leds[i].g = g;
//    leds[i].b = b;
//    FastLED.show();
//  }
//}
