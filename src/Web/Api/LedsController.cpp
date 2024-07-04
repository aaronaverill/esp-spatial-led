#include "LedsController.h"
#include <ArduinoJson.h>
#include "Application.h"
#include "Services/LedDriver.h"

using namespace Services;

namespace Web { namespace Api {
  void LedsController::get(AsyncWebServerRequest *request) {
    LedDriver& leds = LedDriver::getInstance();
    std::vector<Animations::Animation*> animations = leds.getAnimations();

    JsonDocument doc;
    doc["current"] = nullptr;
    JsonArray jsonAnimations = doc["animations"].to<JsonArray>();
    if (animations.size()) {
      doc["current"] = leds.getCurrentAnimationIndex();
      for (Animations::Animation *animation:animations) {
        JsonObject jsonAnimation = jsonAnimations.add<JsonObject>();
        jsonAnimation["name"] = animation->name;
      }
    }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(doc, *response);
    request->send(response);
  }

  void LedsController::setCurrent(AsyncWebServerRequest *request) {
    LedDriver& leds = LedDriver::getInstance();
    uint index = request->getParam("index")->value().toInt();
    leds.setCurrentAnimationIndex(index);
    request->send(200, "text/plain", "OK");
  }
}}