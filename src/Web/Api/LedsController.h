#pragma once
#include <Arduino.h>
#include "ESPAsyncWebServer.h"

namespace Web { namespace Api {
  class LedsController {
    public:
      /**
       * Get info about LED system
       */
      static void get(AsyncWebServerRequest *request);
      /**
       * Set the current LED animation
       */
      static void setCurrent(AsyncWebServerRequest *request);
  };
}}