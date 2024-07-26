// shared-variable.cpp

#include "shared_variable.hpp"
#include <Arduino.h>

uint16_t UrgentOnTime = 1000;
uint8_t custom_ip = 222;
const uint8_t SWITCH_PIN = 23;
bool defaultSwitchState = false;
const char *pinNames[] = {"D3",  "D10", "D4", "D9", "D2", "D1", "-",  "-", "-",
                          "D11", "D12", "-",  "D6", "D7", "D5", "D8", "D0"};
String ssid_STATION = "1011001";
String password_STATION = "dr0wss@p";
String ssid_AP = "Smart_Switch";
String password_AP = "dr0wss@p";
bool isModeStation = true;
bool isActive = false;
const int timeOffsetHour = 5;
const int timeOffsetMin = 45;
const char serverLocation[40] = "http://a-mscs.amrit-p.com.np/api/";
const int MAX_SCHEDULES = 10;
Schedule schedules[MAX_SCHEDULES];
int num_schedules = 0;
bool syncronizeServer = false;
bool syncronizeTime = false;
bool networkSettingChanged = false;
// WiFiManager wifi;
Preferences preferences;
