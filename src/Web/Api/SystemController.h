#pragma once
#include <Arduino.h>
#include "Services/LedDriver.h"
#include "ESPAsyncWebServer.h"

namespace Web { namespace Api {
  /**
   * HTTP Request handler class for API requests related to the system info.
   */
  class SystemController {
    public:
      SystemController(Services::LedDriver& leds) : leds(leds) {}
      /**
       * Get info about system
       */
      void get(AsyncWebServerRequest *request);

    private:
      Services::LedDriver& leds;
  };
}}