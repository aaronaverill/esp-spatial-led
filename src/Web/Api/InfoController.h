#pragma once
#include <Arduino.h>
#include <FS.h>
#include "ESPAsyncWebServer.h"

namespace Web { namespace Api {
  /**
   * HTTP Request handler class for API requests related to the software.
   * Used by the website SPA to fetch global state.
   */
  class InfoController {
    public:
      InfoController(FS& fs) : fs(fs) {}
      /**
       * Get info about system
       */
      void get(AsyncWebServerRequest *request);

    private:
      FS& fs;
  };
}}