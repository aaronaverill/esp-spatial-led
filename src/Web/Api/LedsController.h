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
       * Set an animation setting
       */
      static void setAnimationSetting(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
      /**
       * Set color
       */
      static void setColor(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
      /**
       * Set the led settings
       */
      static void setSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
      /**
       * Set the current LED animation
       */
      static void setPlayIndex(AsyncWebServerRequest *request);
      /**
       * Set a play setting
       */
      static void setPlaySetting(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
      /**
       * Get the current playing frames per second
       */
      static void getFps(AsyncWebServerRequest *request);
      /**
       * Get the current playing r,g,b data
       */
      static void getRgb(AsyncWebServerRequest *request);
  };
}}