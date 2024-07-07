#pragma once
#include <Arduino.h>

struct KeyValuePair {
  KeyValuePair(String key, String value): key(key), value(value) {}
  String key;
  String value;
};