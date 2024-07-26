#include "schedule.hpp"
#include "../shared_variable/shared_variable.hpp"
#include "../test/test.hpp"
#include "../switch/switch.hpp"
#include <EEPROM.h>

void deleteSchedule(int index)
{
    Serial.println("Deleting A Schedule !!");
    schedules[index].isDeleted = true;
    saveSchedulesToEEPROM();
    TEST_schedules_variable_data();
    digitalWrite(SWITCH_PIN, switchStatus());
    num_schedules -- ;
}


void deleteAllSchedules()
{
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

void saveSchedulesToEEPROM()
{
    Serial.println("Saving Schedules to EEPROM !");
  EEPROM.begin(512);
  int address = 0;

  EEPROM.write(address, num_schedules);

  address += sizeof(num_schedules);

  for (int i = 0; i < num_schedules; i++)
  {
    EEPROM.put(address, schedules[i]);
    address += sizeof(schedules[i]);
  }
  EEPROM.end();
}

void loadSchedulesFromEEPROM()
{
    Serial.println("Loading Schedules to EEPROM !");
  EEPROM.begin(512);
  int address = 0;
  EEPROM.get(address, num_schedules);

  if (num_schedules > MAX_SCHEDULES)
  {
    num_schedules = MAX_SCHEDULES;
  }

  address += sizeof(num_schedules);

  for (int i = 0; i < num_schedules; i++)
  {
    EEPROM.get(address, schedules[i]);
    address += sizeof(schedules[i]);
  }
  EEPROM.end();
}
