#include "timing.hpp"
#include "../shared_variable/shared_variable.hpp"
#include <Arduino.h>
#include <NTPClient.h>
#include <RTClib.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>

RTC_DS3231 rtc;

bool initializeTime() { return rtc.begin(); }

DateTime getCurrentTime() { return rtc.now(); }

void displayTime() {
  char format[] = "YYYY MMM DD hh:mm:ss AP";
  Serial.println(getCurrentTime().toString(format));
}

unsigned int getCurrentTimeInSeconds() {
  DateTime time = getCurrentTime();
  uint32_t timeInSeconds =
      time.hour() * 3600 + time.minute() * 60 + time.second();
  return timeInSeconds;
}

bool syncTime() {
  Serial.println("Attempting Time Syncronization !");
  if (!WiFi.isConnected()) {
    Serial.printf(
        "No network connection available. Time synchronization failed. \n");
    return false;
  }
  if (!initializeTime()) {
    Serial.println("Time Syncronization Failed: Timing UNInitialized!");
    return false;
  }

  const long utcOffsetInSeconds =
      timeOffsetHour * 3600 +
      timeOffsetMin * 60; // Your UTC offset in seconds (e.g., 3600 for UTC+1)
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
  timeClient.begin();
  Serial.println("-----------befire--------");

  if (!timeClient.update()) {
    if (!timeClient.forceUpdate()) {
      Serial.println("NTP SYNC Failed!!");
      return false;
    }
  }

  // Serial.println(timeClient.getFormattedTime());

  Serial.println("-----------after--------");
  unsigned long unix_epoch = timeClient.getEpochTime(); // Get epoch time
  Serial.println(unix_epoch);
  DateTime now = DateTime(unix_epoch);
  Serial.printf("\n\nnew time : %d/%d/%d %d:%d \n\n", now.year(), now.month(),
                now.day(), now.hour(), now.minute());
  rtc.adjust(now);

  if (rtc.now() == DateTime(timeClient.getEpochTime())) {
    Serial.printf("Time Syncronization Successful \nRTC Time:");
    Serial.println(getCurrentTime().toString(
        "YYYY/MM/DD hh:mm:ss AP")); // Print synchronization status
    return true;
  } else {
    Serial.printf("Time Syncronization Unsuccessful \n");
    return false;
  }
}
