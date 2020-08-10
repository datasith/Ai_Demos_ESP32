#ifndef _UTIL_H_
#define _UTIL_H_
#include <Arduino.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "secret.h"
#include "pixelate.h"

class MsgHandler
{
  public:
    MsgHandler(WebSocketsClient *webSocket);
    bool begin();
    bool connect();
    bool disconnect();
    bool parse(uint8_t *payload);
    
  private:
    WebSocketsClient *_webSocket;
    StaticJsonDocument<200> _doc;
    void sendTxt(const String msg[], uint8_t n);
    void sendTxt(const char *msg[], uint8_t n);
    bool getPixelData(String msg_str);
};
#endif
