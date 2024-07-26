///shared_variable.hpp

#ifndef SHARED_VARIABLE_HPP
#define SHARED_VARIABLE_HPP
#include <Arduino.h>
#include <Preferences.h>

extern uint16_t UrgentOnTime;
extern uint8_t custom_ip;
extern const uint8_t SWITCH_PIN;
extern bool defaultSwitchState;
extern const char* pinNames[];
extern String ssid_STATION;
extern String password_STATION;
extern String ssid_AP;
extern String password_AP;
extern bool isModeStation;
extern bool isActive;
extern const int timeOffsetHour;
extern const int timeOffsetMin;
extern bool syncronizeServer;
extern bool syncronizeTime;
extern bool networkSettingChanged;
extern const char serverLocation[40];
struct Schedule
{
  int start_time_hour;
  int start_time_min;
  int duration_minutes;
  int duration_seconds;
  bool isDeleted = true;
};
extern const int MAX_SCHEDULES;
// extern Schedule schedules[MAX_SCHEDULES];
extern Schedule schedules[];
extern int num_schedules;


extern Preferences preferences;

#endif