#pragma once
#include <Arduino.h>
#include "ESPAsyncWebServer.h"

namespace Web { namespace Api {
  class LedsController {
    public:
      /**
       * Set the current LED animation
       */
      static void setCurrent(AsyncWebServerRequest *request);
  };
}}