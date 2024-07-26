#include "network.hpp"
#include "../shared_variable/shared_variable.hpp"
#include <WiFi.h>

void establishNetwork(bool reconnect_WiFi) {
  ssid_STATION = preferences.getString("ssid_STATION", ssid_STATION);
  password_STATION =
      preferences.getString("password_STATION", password_STATION);
  ssid_AP = preferences.getString("ssid_AP", ssid_AP);
  password_AP = preferences.getString("password_AP", password_AP);
  Serial.printf(
      "------Eastablish Network ()---------\nStation -> %s %s\nAP -> %s : %s\n",
      ssid_STATION, password_STATION, ssid_AP, password_AP);

  WiFi.mode(WIFI_AP_STA);
  // starts hotspot

  WiFi.config(IPAddress(), IPAddress(), IPAddress());
  WiFi.begin(ssid_STATION, password_STATION); // start connection
  unsigned long wifiTimeoutMillis = 10000;
  if (reconnect_WiFi == false) {
    Serial.println("Connecting to wifi ..");
    for (int i = 0; i < 30 && WiFi.status() != WL_CONNECTED; i++) {
      delay(500);
      Serial.print(".");
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Station Connection Successful!\nIP : ");
    Serial.println(WiFi.localIP());
    reconnectNetworkWithCustomIP();
    saveCredentialsSTATION();
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Station Connection Unsuccessful!to\nSSID : " +
                   ssid_STATION + "\nPWD : " + password_STATION);
  }
  if (reconnect_WiFi == false) {
    Serial.println("attemptiong starting ap mode");
    if (WiFi.softAP(ssid_AP, password_AP)) {
      Serial.print("Soft ap successful \n IP : ");
      Serial.println(WiFi.softAPIP());
    } else {
      Serial.println("Soft AP unsuccessful!");
    }
  }
}

void reconnectNetworkWithCustomIP() {

  IPAddress dnsServer1(8, 8, 8, 8); // Google DNS server
  IPAddress dnsServer2(8, 8, 4, 4); // Google DNS server
  if (WiFi.status() == WL_CONNECTED) {
    IPAddress IP = WiFi.gatewayIP();
    IP[3] = custom_ip;
    WiFi.config(IP, WiFi.gatewayIP(), WiFi.subnetMask(), dnsServer1,
                dnsServer2);
  }
}

void saveCredentialsSTATION() {
  ssid_STATION = WiFi.SSID();
  password_STATION = WiFi.psk();
  preferences.putString("ssid_STATION", ssid_STATION);
  preferences.putString("password_STATION", password_STATION);
}
