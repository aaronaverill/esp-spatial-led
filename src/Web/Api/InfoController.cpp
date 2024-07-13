#include "InfoController.h"
#include <ArduinoJson.h>
#include "Application.h"
#include "Services/LedDriver.h"
#include "Store/LedLayout.h"

using namespace Services;

namespace Web { namespace Api {
  void InfoController::get(AsyncWebServerRequest *request) {
    LedDriver& leds = LedDriver::getInstance();
    const std::vector<Animations::Animation*>& animations = leds.getAnimations();

    JsonDocument doc;
    JsonObject jsonLeds = doc["leds"].to<JsonObject>();
    jsonLeds["count"] = leds.getLedCount();
    jsonLeds["layout"]["config"] = Store::LedLayout::readLayout();
    jsonLeds["layout"]["coords"] = Store::LedLayout::readCoordinates();

    JsonArray jsonColors = jsonLeds["colors"].to<JsonArray>();
    for(CRGB rgb:leds.getColors()) {
      JsonObject jsonColor = jsonColors.add<JsonObject>();
      jsonColor["r"] = rgb.red;
      jsonColor["g"] = rgb.green;
      jsonColor["b"] = rgb.blue;
    }

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
        Web::UI::FieldsInfo fields(jsonAnimationFields);
        animation->getFields(fields);
        JsonObject jsonAnimationSettings = jsonAnimation["settings"].to<JsonObject>();
        animation->getSettings(jsonAnimationSettings);
      }
    }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(doc, *response);
    request->send(response);
  }
}}