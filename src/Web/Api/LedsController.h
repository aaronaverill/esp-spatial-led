#pragma once
#include <Arduino.h>
#include "ESPAsyncWebServer.h"

namespace Web { namespace Api {
  /**
   * HTTP Request handler class for API requests related to the led service.
   */
  class LedsController {
    public:
      /**
       * Set the current LED animation
       */
      static void setPlayIndex(AsyncWebServerRequest *request);
      /**
       * Set a play setting
       */
      static void setPlaySetting(AsyncWebServerRequest *request);
      /**
       * Set an animation setting
       */
      static void setAnimationSetting(AsyncWebServerRequest *request);
      /**
       * Get the current playing frames per second
       */
      static void getFps(AsyncWebServerRequest *request);
  };
}}