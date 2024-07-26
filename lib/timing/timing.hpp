#ifndef TIMING_HPP
#define TIMING_HPP
#include <RTClib.h>
unsigned int getCurrentTimeInSeconds();
DateTime getCurrentTime();
bool syncTime();
extern RTC_DS3231 rtc;
bool initializeTime();
void displayTime();

#endif