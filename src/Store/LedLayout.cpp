#include "LedLayout.h"
#include <ArduinoJson.h>

namespace Store {
  const char* layoutFile = "/led/layout.json";
  const char* coordinatesFile = "/led/coordinates.csv";

  String LedLayout::readLayout(FS& fs) {
    if (!fs.exists(layoutFile)) return "";
    File file = fs.open(layoutFile, "r");
    String content = file.readString();
    file.close();
    return content;
  }

  String LedLayout::readCoordinates(FS& fs) {
    if (!fs.exists(coordinatesFile)) return "";
    File file = fs.open(coordinatesFile, "r");
    String content = file.readString();
    file.close();
    return content;
  }

  void LedLayout::write(FS& fs, const char *layout, const char *coordinates) {
    fs.mkdir("/led");
    
    File file = fs.open(layoutFile, "w");
    if (layout) {
      file.write((const uint8_t*)layout, strlen(layout));
    }
    file.close();

    file = fs.open(coordinatesFile, "w");
    if (coordinates) {
      file.write((const uint8_t*)coordinates, strlen(coordinates));
    }
    file.close();
  }
}