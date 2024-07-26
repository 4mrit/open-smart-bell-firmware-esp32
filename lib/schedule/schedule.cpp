#include "schedule.hpp"
#include "../shared_variable/shared_variable.hpp"
#include "../switch/switch.hpp"
#include "../test/test.hpp"
#include <EEPROM.h>

void deleteSchedule(int index) {
  Serial.println("Deleting A Schedule !!");
  schedules[index].isDeleted = true;
  saveSchedulesToEEPROM();
  TEST_schedules_variable_data();
  digitalWrite(SWITCH_PIN, switchStatus());
  num_schedules--;
}

void deleteAllSchedules() {
  Serial.println(" Warning !! Deleting All Schedules !!");
  EEPROM.begin(512);
  EEPROM.write(0, num_schedules);

  for (int i = 0; i < MAX_SCHEDULES; i++)
    schedules[i].isDeleted = true;

  digitalWrite(SWITCH_PIN, defaultSwitchState);
  saveSchedulesToEEPROM();
  EEPROM.end();
  num_schedules = 0;
}

void saveSchedulesToEEPROM() {
  Serial.println("Saving Schedules to EEPROM !");
  saveSchedulesToPreferences();
  return;
  EEPROM.begin(512);
  int address = 0;

  EEPROM.write(address, num_schedules);

  address += sizeof(num_schedules);

  for (int i = 0; i < num_schedules; i++) {
    EEPROM.put(address, schedules[i]);
    address += sizeof(schedules[i]);
  }
  EEPROM.end();
}

void loadSchedulesFromEEPROM() {
  loadSchedulesFromPreferences();
  return;
  Serial.println("Loading Schedules to EEPROM !");
  EEPROM.begin(512);
  int address = 0;
  EEPROM.get(address, num_schedules);

  if (num_schedules > MAX_SCHEDULES) {
    num_schedules = MAX_SCHEDULES;
  }

  address += sizeof(num_schedules);

  for (int i = 0; i < num_schedules; i++) {
    EEPROM.get(address, schedules[i]);
    address += sizeof(schedules[i]);
  }
  EEPROM.end();
}

void saveSchedulesToPreferences() {
  Serial.println("Saving Schedules to Preferences!");

  preferences.begin("schedules",
                    false); // Open the preferences with a namespace
  preferences.putUInt("num_schedules", num_schedules);

  for (int i = 0; i < num_schedules; i++) {
    String key = "schedule_" + String(i);
    preferences.putBytes(key.c_str(), &schedules[i], sizeof(Schedule));
  }

  preferences.end();
}

void loadSchedulesFromPreferences() {
  Serial.println("Loading Schedules from Preferences!");

  preferences.begin("schedules", true); // Open the preferences with a namespace
  int stored_num_schedules = preferences.getUInt("num_schedules", 0);

  for (int i = 0; i < stored_num_schedules; i++) {
    String key = "schedule_" + String(i);
    preferences.getBytes(key.c_str(), &schedules[i], sizeof(Schedule));
  }

  preferences.end();
}
