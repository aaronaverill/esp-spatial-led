#include "InfoController.h"
#include <ArduinoJson.h>
#include "Application.h"
#include "Store/LedLayout.h"

using namespace Services;

namespace Web { namespace Api {
  void InfoController::get(AsyncWebServerRequest *request) {
    const std::vector<Animations::Animation*>& animations = leds.getAnimations();

    JsonDocument doc;
    JsonObject jsonLeds = doc["leds"].to<JsonObject>();
    jsonLeds["count"] = leds.getLedCount();
    jsonLeds["layout"]["config"] = Store::LedLayout::readLayout(fs);
    jsonLeds["layout"]["coords"] = Store::LedLayout::readCoordinates(fs);

    JsonArray jsonColors = jsonLeds["colors"].to<JsonArray>();
    for(CRGB rgb:leds.getColors()) {
      JsonArray jsonColor = jsonColors.add<JsonArray>();
      jsonColor.add(rgb.r);
      jsonColor.add(rgb.g);
      jsonColor.add(rgb.b);
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