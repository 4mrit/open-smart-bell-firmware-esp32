// -----------------------//
#include "subscription.hpp"
//------------------------//
#include "../functions/functions.hpp"
#include "../network/network.hpp"
#include "../server/include/server.hpp"
#include "../shared_variable/shared_variable.hpp"
#include <ArduinoJson.h>
#include <AsyncHTTPRequest_Generic.h>
#include <HTTPClient.h>

bool subscriptionStatus() {
  HTTPClient http;
  WiFiClient wifiClient;
  bool status;
  bool expired;
  StaticJsonDocument<256> response;
  StaticJsonDocument<48> request;
  const char *mac_address = "";
  int expiry_date_year;
  int expiry_date_month;
  int expiry_date_day;
  bool isExpiredLocal = true;
  String postData;
  time_t now = time(nullptr);
  struct tm *currentDate = localtime(&now);
  tm expiry_date = {};
  // Serial.printf("\nFUNCTION-IS_SAVED_SUBSCRIPTION_ACTIVE\nExpiration Year :
  // %d\nExpiration Month : %d\nExpiration Day:
  // %d\n",expiry_date_year,expiry_date_month,expiry_date_day);
  // Serial.printf("------------\n Year : %d\n Month : %d\n Day:
  // %d",timeinfo->tm_year+1900,timeinfo->tm_mon+ 1,timeinfo->tm_mday);

  //----Format_of_request-------------//
  //  {
  //    "client_mac_address": "40:F5:20:23:01:2E";
  //  }
  //----------------------------------//

  //-----format of response---------//
  // {
  // "status": true,
  // "expired": true,
  // "mac_address": "40:F5:20:23:01:2E",ssresponse["AP_password"]
  // "expiry_date": {
  //     "year": 2023,
  //     "month": 4,
  //     "day": 23
  // }
  //---------------------------------//

  http.begin(wifiClient,
             "http://a-mscs.amrit-p.com.np/api/get_expiry_date.php");
  http.addHeader("Content-Type", "application/json");

  request["client_mac_address"] = WiFi.macAddress();
  serializeJson(request, postData);

  // problem code
  int httpResponseCode = http.POST(postData);

  Serial.println(WiFi.macAddress());

  // Check for successful POST request
  if (httpResponseCode > 0) {
    Serial.printf("HTTP POST request success, response code: %d\n",
                  httpResponseCode);
    DeserializationError error = deserializeJson(response, http.getString());

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return false;
    }
    Serial.println(http.getString());
    status = response["status"];                        // true -> record exists
    expired = response["expired"];                      // false
    mac_address = response["mac_address"];              // "40:F5:20:23:01:2E"
    expiry_date_year = response["expiry_date"]["year"]; // 2023
    expiry_date_month = response["expiry_date"]["month"]; // 4
    expiry_date_day = response["expiry_date"]["day"];     // 23

    Serial.println("Storing Expiry Date in Memory (preferences)");
    preferences.putUShort("exp_date_year", expiry_date_year);
    preferences.putUShort("exp_date_month", expiry_date_month);
    preferences.putUShort("exp_date_day", expiry_date_day);

    String serverAPPassord = response["AP_password"];

    if (!serverAPPassord.equals(preferences.getString("password_AP"))) {
      Serial.printf("AP password Updated!!!");
      preferences.putString("password_AP", serverAPPassord);
      // server.client().flush();
      establishNetwork(0);
    }
  } else {
    Serial.printf("HTTP POST request failed, error code: %d\n",
                  httpResponseCode);
    Serial.println(http.errorToString(httpResponseCode));
  }
  http.end();

  // time_t now = time(nullptr);
  // struct tm *timeinfo = localtime(&now);

  expiry_date.tm_year =
      preferences.getUShort("exp_date_year", 0) - 1900; // Set the year
  expiry_date.tm_mon =
      preferences.getUShort("exp_date_month", 0) - 1; // Set the month (0-11)
  expiry_date.tm_mday =
      preferences.getUShort("exp_date_day", 0) + 1; // Set the day

  // difftime(time_1,time_2) => time_1 - time_2 (in seconds)

  // bool isActiveLocal = (expiry_date_year > timeinfo->tm_year ||
  //                       (expiry_date_year == timeinfo->tm_year &&
  //                       expiry_date_month > timeinfo->tm_mon) ||
  //                       (expiry_date_year == timeinfo->tm_year &&
  //                       expiry_date_month == timeinfo->tm_mon &&
  //                       expiry_date_day > timeinfo->tm_mday));

  bool isActiveLocal = difftime(mktime(&expiry_date), mktime(currentDate)) >= 0;

  if (status == true && expired == false &&
      macAddressinDecimal().equals(mac_address) && isActiveLocal == true) {
    Serial.println("Subscription Active");
    preferences.putBool("exp_date_set", HIGH);
    preferences.putUShort("exp_date_year", expiry_date_year);
    preferences.putUShort("exp_date_month", expiry_date_month);
    preferences.putUShort("exp_date_day", expiry_date_day);

    Serial.printf("Status: %d\n", status);
    Serial.printf("Expired: %d \n", expired);
    Serial.printf("Mac Address: %s\n", mac_address);
    Serial.printf("Expiry date year: %d\n", expiry_date_year);
    Serial.printf("Expiry date month: %d\n", expiry_date_month);
    Serial.printf("Expiry date day: %d\n", expiry_date_day);

    // isActive = true;
    return true;
  } else {
    Serial.printf("Status: %d\n", status);
    Serial.printf("Expired: %d \n", expired);
    Serial.printf("Expiry date year: %d\n", expiry_date_year);
    Serial.printf("Expiry date month: %d\n", expiry_date_month);
    Serial.printf("Expiry date day: %d\n", expiry_date_day);

    preferences.putBool("exp_date_set", LOW);
    preferences.putUShort("exp_date_year", 0);
    preferences.putUShort("exp_date_month", 0);
    preferences.putUShort("exp_date_day", 0);
    Serial.println("Subscription Expired");
    // isActive = false;
    return false;
  }
}
bool isSavedSubscriptionActive() {
  time_t now = time(nullptr);
  struct tm *currentDate = localtime(&now);

  uint16_t expiry_date_year = preferences.getUShort("exp_date_year", 0);
  uint8_t expiry_date_month = preferences.getUShort("exp_date_month", 0);
  uint8_t expiry_date_day = preferences.getUShort("exp_date_day", 0);

  // Serial.printf("\nFUNCTION-IS_SAVED_SUBSCRIPTION_ACTIVE\nExpiration Year :
  // %d\nExpiration Month : %d\nExpiration Day:
  // %d\n",expiry_date_year,expiry_date_month,expiry_date_day);
  // Serial.printf("------------\n Year : %d\n Month : %d\n Day:
  // %d",timeinfo->tm_year+1900,timeinfo->tm_mon+ 1,timeinfo->tm_mday);

  tm expiry_date = {};
  expiry_date.tm_year =
      preferences.getUShort("exp_date_year", 0) - 1900; // Set the year
  expiry_date.tm_mon =
      preferences.getUShort("exp_date_month", 0) - 1; // Set the month (0-11)
  expiry_date.tm_mday =
      preferences.getUShort("exp_date_day", 0) + 1; // Set the day

  // difftime(time_1,time_2) => time_1 - time_2 (in seconds)
  bool result = difftime(mktime(&expiry_date), mktime(currentDate)) >= 0;
  return result;

  // bool response = difftime(mktime(&expiry_date),mktime(currentDate)) >=0;
  // Serial.printf("Comparisionn responsne : %d\n", response);
}
