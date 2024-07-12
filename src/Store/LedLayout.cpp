#include "LedLayout.h"
#include <ArduinoJson.h>
#include "Services/FileSystem.h"
#include "Services/LedDriver.h"

using namespace Services;

namespace Store {
  const char* layoutFile = "/led/layout.json";
  const char* coordinatesFile = "/led/coordinates.csv";

  String LedLayout::readLayout() {
    FileSystem& fs = FileSystem::getInstance();

    if (!fs.exists(layoutFile)) return "";
    File file = fs.open(layoutFile, "r");
    String content = file.readString();
    file.close();
    return content;
  }

  String LedLayout::readCoordinates() {
    FileSystem& fs = FileSystem::getInstance();

    if (!fs.exists(coordinatesFile)) return "";
    File file = fs.open(coordinatesFile, "r");
    String content = file.readString();
    file.close();
    return content;
  }

  void LedLayout::write(const char *layout, const char *coordinates) {
    FileSystem& fs = FileSystem::getInstance();
    File file = fs.open(layoutFile, "w");
    if (layout) {
      file.write(layout);
    }
    file.close();

    file = fs.open(coordinatesFile, "w");
    if (coordinates) {
      file.write(coordinates);
    }
    file.close();
  }
}