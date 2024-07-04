#pragma once
#include <Arduino.h>
#include "ESPAsyncWebServer.h"

namespace Web { namespace Api {
  class LedsController {
    public:
      static void get(AsyncWebServerRequest *request);
      static void setCurrent(AsyncWebServerRequest *request);
  };
}}