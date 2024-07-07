#pragma once
#include <Arduino.h>
#include "ESPAsyncWebServer.h"

namespace Web { namespace Api {
  class InfoController {
    public:
      /**
       * Get info about system
       */
      static void get(AsyncWebServerRequest *request);
  };
}}