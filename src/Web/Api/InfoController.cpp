#include "InfoController.h"
#include <ArduinoJson.h>
#include "Application.h"
#include "Services/LedDriver.h"
#include "Store/LedCoordinates.h"

using namespace Services;

namespace Web { namespace Api {
  void InfoController::get(AsyncWebServerRequest *request) {
    LedDriver& leds = LedDriver::getInstance();
    std::vector<Animations::Animation*> animations = leds.getAnimations();

    JsonDocument doc;
    JsonObject jsonLeds = doc["leds"].to<JsonObject>();
    jsonLeds["count"] = leds.getLedCount();
    jsonLeds["coordinates"]["config"] = Store::LedCoordinates::readConfig();
    jsonLeds["coordinates"]["values"] = Store::LedCoordinates::readValues();
    JsonObject jsonPlay = jsonLeds["play"].to<JsonObject>();
    JsonObject jsonSettings = jsonPlay["settings"].to<JsonObject>();
    jsonSettings["brightness"] = leds.getBrightness();
    jsonPlay["index"] = nullptr;
    JsonArray jsonAnimations = jsonLeds["animations"].to<JsonArray>();
    if (animations.size()) {
      jsonPlay["index"] = leds.getCurrentAnimationIndex();
      for (Animations::Animation *animation:animations) {
        JsonObject jsonAnimation = jsonAnimations.add<JsonObject>();
        jsonAnimation["name"] = animation->name;
        JsonArray jsonAnimationFields = jsonAnimation["fields"].to<JsonArray>();
        animation->getFields(jsonAnimationFields);
        JsonObject jsonAnimationSettings = jsonAnimation["settings"].to<JsonObject>();
        animation->getSettings(jsonAnimationSettings);
      }
    }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(doc, *response);
    request->send(response);
  }
}}