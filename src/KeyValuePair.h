#pragma once
#include <Arduino.h>

/**
 * Represents a key, value pair of the types specified
 */
template <typename KeyType, typename ValueType> struct KeyValuePair {
  KeyValuePair(const KeyType& key, const ValueType& value): key(key), value(value) {}
  KeyType key;
  ValueType value;
};