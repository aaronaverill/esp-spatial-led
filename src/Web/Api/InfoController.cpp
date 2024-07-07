#include "InfoController.h"
#include <ArduinoJson.h>
#include "Application.h"
#include "Services/LedDriver.h"

using namespace Services;

namespace Web { namespace Api {
  void InfoController::get(AsyncWebServerRequest *request) {
    LedDriver& leds = LedDriver::getInstance();
    std::vector<Animations::Animation*> animations = leds.getAnimations();

    JsonDocument doc;
    JsonObject jsonLeds = doc["leds"].to<JsonObject>();
    JsonObject jsonPlay = jsonLeds["play"].to<JsonObject>();
    jsonPlay["index"] = nullptr;
    JsonArray jsonAnimations = jsonLeds["animations"].to<JsonArray>();
    if (animations.size()) {
      jsonPlay["index"] = leds.getCurrentAnimationIndex();
      for (Animations::Animation *animation:animations) {
        JsonObject jsonAnimation = jsonAnimations.add<JsonObject>();
        jsonAnimation["name"] = animation->name;
      }
    }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(doc, *response);
    request->send(response);
  }
}}