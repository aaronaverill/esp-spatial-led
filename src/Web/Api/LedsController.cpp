#include "LedsController.h"
#include <ArduinoJson.h>
#include "Application.h"
#include "Animations/Animation.h"
#include "Store/LedLayout.h"
#include "Store/LedSettings.h"

using namespace Services;

namespace Web { namespace Api {
  void LedsController::setAnimationSetting(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data);
    if (error) {
      request->send(500);
    }

    JsonVariant animationIndex = doc["index"];
    if (!animationIndex.isNull()) {
      const std::vector<Animations::Animation*>& animations = leds.getAnimations();
      if (animationIndex < animations.size()) {
        animations[animationIndex]->setSettings(doc.as<JsonObject>());
      }
      Store::LedSettings::write(fs, leds);
    }
    request->send(200, "text/plain", "OK");
  }

  void LedsController::setColor(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data);
    if (error) {
      request->send(500);
    }

    JsonVariant colorIndex = doc["index"];
    JsonVariant rgb = doc["rgb"];
    if (!colorIndex.isNull() && rgb) {
      if (colorIndex < leds.getColors().size()) {
        leds.setColor(colorIndex, rgb[0], rgb[1] ,rgb[2]);
      }
      Store::LedSettings::write(fs, leds);
    }
    request->send(200, "text/plain", "OK");
  }

  void LedsController::addPalette(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data);
    if (error) {
      request->send(500);
    }
    
    JsonVariant name = doc["name"];
    JsonVariant stopsArray = doc["stops"];
    if (!name.isNull() && !stopsArray.isNull()) {
      std::vector<Palette::GradientStop> stops;
      for(size_t i = 0; i < stopsArray.size(); i++) {
        stops.push_back(Palette::GradientStop(stopsArray[i][0], stopsArray[i][1], stopsArray[i][2], stopsArray[i][3]));
      }
      leds.addPalette(name.as<String>(), stops);
      Store::LedSettings::write(fs, leds);
    }
    request->send(200, "text/plain", "OK");
  }

  void LedsController::setPalette(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data);
    if (error) {
      request->send(500);
    }

    JsonVariant paletteIndex = doc["index"];
    JsonVariant name = doc["name"];
    JsonVariant stopsArray = doc["stops"];
    if (!paletteIndex.isNull() && (!name.isNull() || !stopsArray.isNull())) {
      if (paletteIndex < leds.getPalettes().size()) {
        std::vector<Palette::GradientStop> stops;
        for(size_t i = 0; i < stopsArray.size(); i++) {
          stops.push_back(Palette::GradientStop(stopsArray[i][0], stopsArray[i][1], stopsArray[i][2], stopsArray[i][3]));
        }
        leds.setPalette(paletteIndex, name.as<String>(), stops);
      }
      Store::LedSettings::write(fs, leds);
    }
    request->send(200, "text/plain", "OK");
  }

  void LedsController::setSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data);
    if (error) {
      request->send(500);
    }

    bool writeSettings = false;
    JsonVariant chipset = doc["chipset"];
    if (!chipset.isNull()) {
      leds.setChipset(chipset);
      writeSettings = true;
    }
    JsonVariant colorOrder = doc["colorOrder"];
    if (!colorOrder.isNull()) {
      leds.setColorOrder(colorOrder);
      writeSettings = true;
    }
    JsonVariant count = doc["count"];
    if (count) {
      leds.setLedCount(count);
      writeSettings = true;
    }
    JsonVariant coordinates = doc["coordinates"];
    if (coordinates) {
      leds.setLedCoordinates(coordinates);
      Store::LedLayout::write(fs, doc["layout"], doc["coordinates"]);
    }
    if (writeSettings) {
      Store::LedSettings::write(fs, leds);
    }
    request->send(200, "text/plain", "OK");
  }

  void LedsController::setPlayIndex(AsyncWebServerRequest *request) {
    uint index = request->getParam("index")->value().toInt();
    leds.setCurrentAnimationIndex(index);
    Store::LedSettings::write(fs, leds);
    request->send(200, "text/plain", "OK");
  }

  void LedsController::setPlaySetting(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data);
    if (error) {
      request->send(500);
    }

    JsonVariant brightness = doc["brightness"];
    if (brightness) {
      if (brightness >= 253) {
        leds.setBrightness(255);
      } else {
        leds.setBrightness(brightness);
      }
      Store::LedSettings::write(fs, leds);
    }
    request->send(200, "text/plain", "OK");
  }

  void LedsController::getPlayBuffer(AsyncWebServerRequest *request) const {
    size_t ledSize = leds.getLedSize();
    size_t ledCount = leds.getRenderLedCount();

    size_t headerSize = 1 + ledSize;
    size_t bufferSize = ledCount * ledSize;

    // Compiler gives unused-but-set-variable warning even though we are using this in the closure below
    //__attribute__((unused))
    uint8_t header[headerSize];
    memset(header, 0xFF, headerSize);
    header[0] = ledSize;
    // This is a bit messy and should probably be encapsulated into INeoPixelBus to accomodate RGBW
    uint8_t layouts[][3] = {
      {0x00, 0x01, 0x02}, // RGB
      {0x00, 0x02, 0x01}, // RBG
      {0x01, 0x02, 0x00}, // GBR
      {0x01, 0x00, 0x02}, // GRB
      {0x02, 0x01, 0x00}, // BGR
      {0x02, 0x00, 0x01} // BRG
    };
    memcpy(header + 1, layouts[leds.getColorOrder()], 3);
    uint8_t *headerPtr = header;

    size_t size = headerSize + bufferSize;

    LedDriver* ledDriver = &leds;

    AsyncWebServerResponse *response = request->beginResponse("application/octet-stream", size, 
      [ledDriver, ledSize, headerPtr, headerSize, size] (uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
      // Fetch the CRGB data pointer and size within each request to fulfill a buffer chunk from 
      // ESPAsyncWebServer in case  the number of LEDs has been changed and the buffer has been
      // reallocated or resized during the time between when the data is requested and when a 
      // response can be fulfilled. If the number of LEDs has decreased, a bitmap will be created sized
      // to a the width of the original number of LED padded with black at the end.
      // Technically speaking there is a very small window between when the LED buffer is requested
      // and when the memcpy() occurs where the buffer could have been resized and recreated
      // but this is a vanishingly rare occurrence, and worst case it means garbage in the bitmap
      // for a single request.
      const uint8_t* bufferData = ledDriver->getLeds();
      size_t ledCount = ledDriver->getRenderLedCount();
      size_t bufferSize = ledCount * ledSize;

      size_t bytesSent = 0;
      while (index < size && bytesSent < maxLen) {
        size_t chunkSize;
        if (index < headerSize) {
          size_t headerRemaining = headerSize - index;
          chunkSize = min(maxLen - bytesSent, headerRemaining);
          memcpy(buffer + bytesSent, headerPtr + index, chunkSize);

        } else if (index < (headerSize + bufferSize)) {
          size_t rgbRemaining = headerSize + bufferSize - index;
          chunkSize = min(maxLen - bytesSent, rgbRemaining);
          memcpy(buffer + bytesSent, bufferData + index - headerSize, chunkSize);

        } else {
          chunkSize = maxLen - bytesSent;
          memset(buffer + bytesSent, 0, chunkSize);

        }
        index += chunkSize;
        bytesSent += chunkSize;
      }
      return bytesSent; 
    });

    request->send(response);
  }
}}