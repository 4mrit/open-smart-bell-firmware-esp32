#include "test.hpp"
#include "../shared_variable/shared_variable.hpp"

void TEST_schedules_variable_data()
{
  Serial.println("-------TESTING SCHEDULES DATA------------");
  Serial.printf("Index | hour | tMinutes | Duration(M) | Duration(S) | Deleted?\n");
  for (int i = 0; i < MAX_SCHEDULES; i++)  {
    Serial.printf("%d     |  %d  |   %d     |   %d   |   %d   | %d\n", i, schedules[i].start_time_hour, schedules[i].start_time_min,schedules[i].duration_minutes, schedules[i].duration_seconds, schedules[i].isDeleted);
  }
  Serial.println("-----------------------------------------");
}

