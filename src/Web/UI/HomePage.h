#pragma once
#include "ESPAsyncWebServer.h"

namespace Web { namespace UI {
  /**
   * HTTP Request handler class for website requests.
   * Returns an HTML page that is a single page application (SPA) which controls the software.
   */
  class HomePage {
    public:
      static void get(AsyncWebServerRequest *request);
  };
}}