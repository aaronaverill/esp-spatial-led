#pragma once
#include <ESPAsyncWebServer.h>

class BmpFile {
  public:
    static void writeLedData(AsyncWebServerRequest *request);
};