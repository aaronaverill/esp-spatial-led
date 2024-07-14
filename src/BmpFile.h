#pragma once
#include <ESPAsyncWebServer.h>
#include "Services/ILedInfo.h"

class BmpFile {
  public:
    /**
     * Respond to an HTTP request by returning the current LED R,G,B data as an image/bmp
     */
    static void writeLedData(AsyncWebServerRequest *request, const Services::ILedInfo* leds);
};