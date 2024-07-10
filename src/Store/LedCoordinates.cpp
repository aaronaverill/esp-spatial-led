#include "LedCoordinates.h"
#include <ArduinoJson.h>
#include "Services/FileSystem.h"
#include "Services/LedDriver.h"

using namespace Services;

namespace Store {
  const char* configFile = "/led/coordinates.json";
  const char* valuesFile = "/led/coordinates.csv";

  String LedCoordinates::readConfig() {
    FileSystem& fs = FileSystem::getInstance();

    if (!fs.exists(configFile)) return "";
    File file = fs.open(configFile, "r");
    String content = file.readString();
    file.close();
    return content;
  }

  String LedCoordinates::readValues() {
    FileSystem& fs = FileSystem::getInstance();

    if (!fs.exists(valuesFile)) return "";
    File file = fs.open(valuesFile, "r");
    String content = file.readString();
    file.close();
    return content;
  }

  void LedCoordinates::write(const char * config, const char * values) {
    FileSystem& fs = FileSystem::getInstance();
    File file = fs.open(configFile, "w");
    if (config) {
      file.write(config);
    }
    file.close();

    file = fs.open(valuesFile, "w");
    if (values) {
      file.write(values);
    }
    file.close();
  }
}