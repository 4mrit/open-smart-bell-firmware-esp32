#ifndef SCHEDULE_HPP
#define SCHEDULE_HPP

void deleteSchedule(int);
void deleteAllSchedules();
void saveSchedulesToEEPROM();
void loadSchedulesFromEEPROM();
void saveSchedulesToPreferences();
void loadSchedulesFromPreferences();
void debugReadAllPreferences();
#endif
