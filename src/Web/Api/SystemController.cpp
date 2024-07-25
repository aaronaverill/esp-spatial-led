#include "SystemController.h"
#include <ArduinoJson.h>
#include <LittleFS.h>

using namespace Services;

namespace Web { namespace Api {
  void SystemController::get(AsyncWebServerRequest *request) {
    JsonDocument doc;
    doc["Performance"]["Frames per Second"] = leds.getFramesPerSecond();
#ifdef ESP32
    doc["System"]["Chip Model"] = ESP.getChipModel();
#elif ESP8266
    doc["System"]["Chip Model"] = "ESP8266";
#endif
    doc["System"]["Sdk Version"] = ESP.getSdkVersion();
    doc["System"]["Cpu Frequency (MHz)"] = ESP.getCpuFreqMHz();
    doc["Memory"]["Free Heap"] = ESP.getFreeHeap();
    doc["Memory"]["Free Sketch Space"] = ESP.getFreeSketchSpace();
    doc["Memory"]["Sketch Size"] = ESP.getSketchSize();

#ifdef ESP32
    doc["Memory"]["Flash Chip Size"] = String(spi_flash_get_chip_size());
#endif

#ifdef ESP8266
    doc["Memory"]["Max Free Block Size"] = ESP.getMaxFreeBlockSize();
    doc["Memory"]["Flash Chip Size"] = ESP.getFlashChipRealSize();

    FSInfo fs_info;
    if (LittleFS.info(fs_info)) {
      doc["File System"]["Total Size"] = fs_info.totalBytes;
      doc["File System"]["Used"] = fs_info.usedBytes;
      doc["File System"]["Free"] = fs_info.totalBytes - fs_info.usedBytes;
    }
#endif

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(doc, *response);
    request->send(response);
  }
}}