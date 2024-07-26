#ifndef SERVER_HPP
#define SERVER_HPP

// #include <Arduino.h>
// #ifdef ESP32
// #elif defined(ESP8266)
// #include <ESP8266WiFi.h>
// #include <ESPAsyncTCP.h>
// #endif

#include <ESPAsyncWebServer.h>
// #include "../../.pio/libdeps/nodemcuv2/ESP Async WebServer/src/ESPAsyncWebServer.h"

void startWebServer();
void startWebServer_ACTIVE();
void startWebServer_EXPIRED();

void handleRoot_GET(AsyncWebServerRequest*);
void handleExpiredRoot_GET(AsyncWebServerRequest*);
void handleRoot_POST(AsyncWebServerRequest*);
void handleDeleteSchedule_POST(AsyncWebServerRequest*);
void handleDeleteAllSchedule_POST(AsyncWebServerRequest*);
void handleSettings_GET(AsyncWebServerRequest*);
void handleSettings_POST(AsyncWebServerRequest*);
void handleToogleSwitchState_POST(AsyncWebServerRequest*);
void handleSyncTime_POST(AsyncWebServerRequest*);
void handleRecheckSubscription_POST(AsyncWebServerRequest*);
void handleUrgentOn_POST(AsyncWebServerRequest*);
void handleSyncServer_POST(AsyncWebServerRequest*);
void handleRestart_POST(AsyncWebServerRequest*);
void handleMessage(AsyncWebServerRequest*,String);

extern AsyncWebServer server;
#endif