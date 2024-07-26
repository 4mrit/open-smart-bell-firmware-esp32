#include "include/server.hpp"
#include "../functions/functions.hpp"
#include "../network/network.hpp"
#include "../schedule/schedule.hpp"
#include "../shared_variable/shared_variable.hpp"
#include "../subscription/subscription.hpp"
#include "../switch/switch.hpp"
#include "../test/test.hpp"
#include "../timing/timing.hpp"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

AsyncWebServer server(80);
String parseSettingsPage(const String &);
String parseRootPage(const String &);

void startWebServer() {
  bool expiry_date_set = preferences.getBool("expiry_date_set");
  Serial.printf("saved sub: %s <-> ",
                isSavedSubscriptionActive() ? "true" : "false");
  Serial.printf("expiry date sub: %s", expiry_date_set ? "true" : "false");
  if ((expiry_date_set && isSavedSubscriptionActive()) ||
      subscriptionStatus() == true) {
    isActive = true;
    startWebServer_ACTIVE();
  } else {
    // isActive = false;
    // startWebServer_EXPIRED();
    isActive = true;
    startWebServer_ACTIVE();
  }
}

void startWebServer_ACTIVE() {
  server.on("/", HTTP_GET,
            [](AsyncWebServerRequest *request) { handleRoot_GET(request); });

  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request) {
    handleSettings_GET(request);
  });

  server.on("/", HTTP_POST,
            [](AsyncWebServerRequest *request) { handleRoot_POST(request); });

  server.on("/delete-schedule", HTTP_POST, [](AsyncWebServerRequest *request) {
    handleDeleteSchedule_POST(request);
  });

  server.on("/delete-all-schedule", HTTP_POST,
            [](AsyncWebServerRequest *request) {
              handleDeleteAllSchedule_POST(request);
            });

  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request) {
    handleSettings_GET(request);
  });

  server.on("/settings", HTTP_POST, [](AsyncWebServerRequest *request) {
    handleSettings_POST(request);
  });

  server.on("/toogle-switch-state", HTTP_POST,
            [](AsyncWebServerRequest *request) {
              handleToogleSwitchState_POST(request);
            });

  server.on("/sync-time", HTTP_POST, [](AsyncWebServerRequest *request) {
    handleSyncTime_POST(request);
  });

  server.on("/urgent-on", HTTP_POST, [](AsyncWebServerRequest *request) {
    handleUrgentOn_POST(request);
  });

  server.on("/sync-server", HTTP_POST, [](AsyncWebServerRequest *request) {
    handleSyncServer_POST(request);
  });

  server.on("/recheck-subscription", HTTP_GET,
            [](AsyncWebServerRequest *request) { handleRoot_GET(request); });

  server.on("/restart", HTTP_POST, [](AsyncWebServerRequest *request) {
    handleRestart_POST(request);
  });

  server.on("/restart", HTTP_GET, [](AsyncWebServerRequest *request) {
    handleSettings_GET(request);
  });

  server.begin();
  Serial.println("light Scheduling HTTP Server started");
}
void startWebServer_EXPIRED() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    handleExpiredRoot_GET(request);
  });

  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request) {
    handleSettings_GET(request);
  });

  server.on("/settings", HTTP_POST, [](AsyncWebServerRequest *request) {
    handleSettings_POST(request);
  });

  server.on("/recheck-subscription", HTTP_POST,
            [](AsyncWebServerRequest *request) {
              handleRecheckSubscription_POST(request);
            });

  server.on("/toogle-switch-state", HTTP_POST,
            [](AsyncWebServerRequest *request) {
              handleToogleSwitchState_POST(request);
            });
  server.on("/sync-time", HTTP_POST, [](AsyncWebServerRequest *request) {
    handleSyncTime_POST(request);
  });
  server.on("/sync-server", HTTP_POST, [](AsyncWebServerRequest *request) {
    handleSyncServer_POST(request);
  });

  server.on("/restart", HTTP_POST, [](AsyncWebServerRequest *request) {
    handleRestart_POST(request);
  });

  server.on("/restart", HTTP_GET, [](AsyncWebServerRequest *request) {
    handleSettings_GET(request);
  });
  server.begin();
  Serial.println("Subscription Expired Server");
}

void handleRoot_GET(AsyncWebServerRequest *request) {
  String filePath = "/root_active.html";
  if (!SPIFFS.exists(filePath)) {
    request->send(404, "text/html", "Page Not Found!!");
  }
  request->send(SPIFFS, filePath, String(), false, parseRootPage);
}

void handleExpiredRoot_GET(AsyncWebServerRequest *request) {
  String filePath = "/root_expired.html";
  if (!SPIFFS.exists(filePath)) {
    request->send(404, "text/html", "Page Not Found!!");
  }
  request->send(SPIFFS, filePath, String());
}

void handleRoot_POST(AsyncWebServerRequest *request) {
  Serial.println("handling / post request");
  TEST_schedules_variable_data();
  String start_time = "";
  String start_time_hour = "";
  String start_time_min = "";
  String duration_minutes = "";
  String duration_seconds = "";

  for (int i = 0; i < MAX_SCHEDULES; i++) {
    if (!request->hasArg("start_time")) {
      break;
    }

    start_time = request->arg("start_time");
    duration_minutes = request->arg("duration_minutes");
    duration_seconds = request->arg("duration_seconds");

    char timeCharArray[6]; // Allocate space for the C-style string

    // Copy the String to the character array
    start_time.toCharArray(timeCharArray, sizeof(timeCharArray));

    String start_time_hour = strtok(timeCharArray, ":");
    String start_time_min = strtok(NULL, ":");

    Serial.printf("Raw Start Time from Request : %s", start_time);

    Serial.printf("\n\n--------------------\n"
                  "start_time_hr : %s "
                  "start_time_min : %s "
                  "duartion_min : %s "
                  "duration_sec : %s ",
                  start_time_hour, start_time_min, duration_minutes,
                  duration_seconds);

    if (start_time_hour != "" &&
        (duration_minutes != "" || duration_seconds != "") &&
        schedules[i].isDeleted) {
      Serial.printf("Writing Schedule to Index : %d\n", i);
      schedules[i].start_time_hour = start_time_hour.toInt();
      schedules[i].start_time_min = start_time_min.toInt();
      schedules[i].duration_minutes = duration_minutes.toInt();
      schedules[i].duration_seconds = duration_seconds.toInt();
      schedules[i].isDeleted = false;
      break;
    }
  }

  num_schedules++;
  saveSchedulesToEEPROM();
  TEST_schedules_variable_data();
  request->redirect("/");
}
void handleDeleteSchedule_POST(AsyncWebServerRequest *request) {
  String index = request->arg("index");
  Serial.printf("handling deletion of index : %s\n", index);
  deleteSchedule(index.toInt());
  request->redirect("/");
}
void handleDeleteAllSchedule_POST(AsyncWebServerRequest *request) {
  deleteAllSchedules();
  request->redirect("/");
}

void handleSettings_GET(AsyncWebServerRequest *request) {
  String filePath = "/settings.html";
  if (!SPIFFS.exists(filePath)) {
    request->send(404, "text/html", "Page Not Found!!");
  }
  request->send(SPIFFS, filePath, "text/html", false, parseSettingsPage);
}

void handleSettings_POST(AsyncWebServerRequest *request) {
  Serial.println("-----------server-POST-Settings--------------");
  String network_mode = request->arg("network_mode");
  String wifi_ssid = request->arg("ssid");
  String wifi_password = request->arg("password");
  String ip_address = request->arg("ip");

  if (network_mode != "") {
    isModeStation = network_mode == "station";
    Serial.printf("Network is Station : %d\n", isModeStation);
  }

  if (ip_address != "") {
    custom_ip = ip_address.toInt();
    Serial.printf("IP set to : %d\n", custom_ip);
  }

  if (wifi_ssid != "") {
    if (network_mode == "ap") {
      ssid_AP = wifi_ssid;
      preferences.putString("ssid_AP", ssid_AP);
      Serial.printf("AP SSID set to : %s\n", ssid_AP);
    } else {
      ssid_STATION = wifi_ssid;
      preferences.putString("ssid_STN", ssid_STATION);
      Serial.printf("STATION SSID set to : %s\n", ssid_STATION);
    }
  }

  if (wifi_password != "") {
    if (network_mode == "ap") {
      // password_AP = wifi_password;
      // preferences.putString("password_AP", password_AP);
      // Serial.printf("AP Password set to : %s\n", password_AP);
    } else {
      password_STATION = wifi_password;
      preferences.putString("password_STN", password_STATION);
      Serial.printf("STATION Password set to : %s\n", password_STATION);
    }
  }

  Serial.print("setting saved");
  request->send(200, "text/html", "saved");
  // flush any pending data to the client
  // server.client().flush();

  // server.stop();
  // delay(10);
  // -----v imp-------//
  networkSettingChanged = true;
}

void handleToogleSwitchState_POST(AsyncWebServerRequest *request) {
  toggleSwitchState();
  request->redirect("/settings");
}

void handleSyncTime_POST(AsyncWebServerRequest *request) {
  syncronizeTime = true;
  request->redirect("/settings");
}

void handleUrgentOn_POST(AsyncWebServerRequest *request) {
  digitalWrite(SWITCH_PIN, !defaultSwitchState);
  request->redirect("/");

  time_t completion_time = millis() + UrgentOnTime;
  while (millis() < completion_time)
    ;

  if (isSavedSubscriptionActive()) {
    digitalWrite(SWITCH_PIN, switchStatus());
  } else {
    digitalWrite(SWITCH_PIN, defaultSwitchState);
  }
}

void handleRecheckSubscription_POST(AsyncWebServerRequest *request) {
  syncronizeServer = true;
  request->send(200, "text/html", "Evaluating Subscription !!");
}

void handleRestart_POST(AsyncWebServerRequest *request) {
  request->redirect("/");
  ESP.restart();
}

void handleSyncServer_POST(AsyncWebServerRequest *request) {
  Serial.println("handling sync Server");
  syncronizeServer = true;
  request->redirect("/settings");
}
void handleMessage(AsyncWebServerRequest *request, String message) {
  request->send(200, message);
}

String parseSettingsPage(const String &var) {
  IPAddress IP = WiFi.localIP();

  if (var == "CURRENT_TIME") {
    // hh - hour
    // mm - min
    // ss - sec
    // AP - AM/PM
    // cannot directly use toString("hh:mm AP") cause this makes it print same
    // thing for more info read this :
    // https://forum.arduino.cc/t/solved-adafruit-gfx-rtclib-seconds-not-refreshing-updating/926580/5
    char format[] = "hh:mm AP";
    return getCurrentTime().toString(format);
  }

  if (var == "WIFI_SSID") {
    return WiFi.SSID();
  }

  if (var == "WIFI_STATE") {
    String wifi_state = WiFi.status() == WL_CONNECTED
                            ? "<span class='green'>Connected</span>"
                            : "<span class='red'>Disconnected</span>";

    return (wifi_state);
  }

  if (var == "MAC_ADDRESS") {
    return macAddressinDecimal();
  }

  if (var == "IP[0]") {
    IP[0] = IP[0] ? IP[0] : 192;
    return String(IP[0]);
  }

  if (var == "IP[1]") {
    IP[1] = IP[1] ? IP[1] : 168;
    return String(IP[1]);
  }

  if (var == "IP[2]") {
    IP[2] = IP[2] ? IP[2] : 1;
    return String(IP[2]);
  }

  if (var == "IP[3]") {
    return String(IP[3]);
  }

  return String("Error:No Template Processor Found");
}

String parseRootPage(const String &var) {

  if (var == "EXPIRY_DATE") {

    String expiry_date_year = String(preferences.getUShort("exp_date_year"));
    String expiry_date_month = String(preferences.getUShort("exp_date_month"));
    String expiry_date_day = String(preferences.getUShort("exp_date_day"));

    return expiry_date_year + " / " + expiry_date_month + " / " +
           expiry_date_day;
  }

  if (var == "SCHEDULE_ROWS") {
    int start_time_hour;
    String rows = "", format;
    for (int i = 0; i < num_schedules; i++) {
      if (schedules[i].isDeleted)
        continue;

      schedules[i].start_time_hour < 12 ? format = "AM" : format = "PM";
      if (schedules[i].start_time_hour > 12)
        start_time_hour = schedules[i].start_time_hour - 12;
      else
        start_time_hour = schedules[i].start_time_hour;

      rows += R"(
      <tr>
      <td>)" + String(start_time_hour) +
              ":" + String(schedules[i].start_time_min) + " " + format +
              R"(</td>
      <td>)" + String(schedules[i].duration_minutes) +
              " min :" + String(schedules[i].duration_seconds) + R"( sec</td>
      <td>
          <form method='POST' action='/delete-schedule'>
          <input type='hidden' name='index' value=")" +
              i + R"(">
          <button type='submit'>Delete</button>
          </form>
      </td>
      </tr>
      )";
    }
    return rows;
  }

  if (var == "URGENT_TIME") {
    return String(UrgentOnTime / 1000);
  }
  return String("Error:No Template Processor Found");
}
