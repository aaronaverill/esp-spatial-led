#include "SystemController.h"
#include <ArduinoJson.h>
#include <LittleFS.h>

using namespace Services;

namespace Web { namespace Api {
  void SystemController::get(AsyncWebServerRequest *request) {
    JsonDocument doc;
    doc["Performance"]["Frames per Second"] = leds.getFramesPerSecond();
    doc["System"]["Core Version"] = ESP.getCoreVersion();
    doc["System"]["Sdk Version"] = ESP.getSdkVersion();
    doc["System"]["Cpu Frequency (MHz)"] = ESP.getCpuFreqMHz();
    doc["Memory"]["Free Heap"] = ESP.getFreeHeap();
    doc["Memory"]["Max Free Block Size"] = ESP.getMaxFreeBlockSize();
    doc["Memory"]["Flash Chip Size"] = ESP.getFlashChipRealSize();
    doc["Memory"]["Free Sketch Space"] = ESP.getFreeSketchSpace();
    doc["Memory"]["Sketch Size"] = ESP.getSketchSize();

    FSInfo fs_info;
    if (LittleFS.info(fs_info)) {
      doc["File System"]["Total Size"] = fs_info.totalBytes;
      doc["File System"]["Used"] = fs_info.usedBytes;
      doc["File System"]["Free"] = fs_info.totalBytes - fs_info.usedBytes;
    }
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(doc, *response);
    request->send(response);
  }
}}