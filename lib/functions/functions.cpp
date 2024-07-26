#include "functions.hpp"
#include "../shared_variable/shared_variable.hpp"
#include <Arduino.h>
#include <WiFi.h>

String macAddressinDecimal() {
  String decimalMac = "";

  // Remove colons from MAC address
  String sanitizedMac = WiFi.macAddress();
  sanitizedMac.replace(":", "");

  // Convert the entire MAC address from hexadecimal to decimal
  unsigned long long decimalValue = strtoull(sanitizedMac.c_str(), NULL, 16);
  decimalMac = String(decimalValue);

  return decimalMac;
}
