#include "switch.hpp"
#include "../shared_variable/shared_variable.hpp"
#include "../timing/timing.hpp"

bool switchStatus()
{
  unsigned int current_time = getCurrentTimeInSeconds(); // returns in form of minutes eg: 5:1 pm = 1021m
  Serial.print(pinNames[SWITCH_PIN]);

  for (int i = 0; i < num_schedules; i++)
  {
    if (schedules[i].isDeleted)
      continue;
    int start_time = schedules[i].start_time_hour * 3600 + schedules[i].start_time_min *60 ;
    int duration = schedules[i].duration_minutes *60 + schedules[i].duration_seconds;
    // Serial.printf("-------light status--------\ncurrent_time = %d\nstart_time(%d) = %d,%d\n--------------",current_time,i,start_time,start_time+duration);

    if (start_time <= current_time && current_time < start_time + duration)
    {
      Serial.printf(" : ON (%d)  ",!defaultSwitchState);
      return !defaultSwitchState;
    }
  }
  Serial.printf(" : OFF (%d)  ",defaultSwitchState);
  return defaultSwitchState;
}

bool toggleSwitchState(){
    Serial.println("Toggling default switch state");
    defaultSwitchState = !defaultSwitchState;
    preferences.putBool("defaultSwitchState",defaultSwitchState);
    Serial.printf("default state = %s",defaultSwitchState?"HIGH":"LOW");
    return defaultSwitchState;
}

