#pragma once
#include <Arduino.h>
#include "ESPAsyncWebServer.h"

namespace Web { namespace Api {
  class LedAnimationsController {
    public:
      static void getAnimationsList(AsyncWebServerRequest *request) {
        String json = R"(
{
  "animations": [
    {
      "name": "Colorloop"
    },
    {
      "name": "Rainbow"
    },
    {
      "name": "Solid"
    }
  ]
}
    )";
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        response->print(json);
        request->send(response);
      } 
  };
}}