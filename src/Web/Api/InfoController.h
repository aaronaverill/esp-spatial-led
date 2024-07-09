#pragma once
#include <Arduino.h>
#include "ESPAsyncWebServer.h"

namespace Web { namespace Api {
  /**
   * HTTP Request handler class for API requests related to the software.
   * Used by the website SPA to fetch global state.
   */
  class InfoController {
    public:
      /**
       * Get info about system
       */
      static void get(AsyncWebServerRequest *request);
  };
}}