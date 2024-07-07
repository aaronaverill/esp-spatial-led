#pragma once
#include <Arduino.h>
#include "ESPAsyncWebServer.h"

namespace Web { namespace Api {
  class LedsController {
    public:
      /**
       * Set the current LED animation
       */
      static void setPlayIndex(AsyncWebServerRequest *request);
      /**
       * Get the current playing frames per second
       */
      static float getFps(AsyncWebServerRequest *request);
  };
}}