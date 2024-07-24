#pragma once
#include <Arduino.h>
#include <FS.h>
#include "Services/LedDriver.h"
#include "ESPAsyncWebServer.h"

namespace Web { namespace Api {
  /**
   * HTTP Request handler class for API requests related to the led service.
   */
  class LedsController {
    public:
      LedsController(FS& fs, Services::LedDriver& leds) : fs(fs), leds(leds) {}
      /**
       * Set an animation setting
       */
      void setAnimationSetting(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
      /**
       * Set color
       */
      void setColor(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
      /**
       * Set the led settings
       */
      void setSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
      /**
       * Set the current LED animation
       */
      void setPlayIndex(AsyncWebServerRequest *request);
      /**
       * Set a play setting
       */
      void setPlaySetting(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
      /**
       * Get the current playing r,g,b data
       */
      void getRgb(AsyncWebServerRequest *request) const;

    private:
      FS& fs;
      Services::LedDriver& leds;
  };
}}