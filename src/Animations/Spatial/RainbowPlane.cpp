#include "RainbowPlane.h"
#include "Algorithm/Matrix3D.h"

#include <FastLED.h>

namespace Animations { namespace Spatial {
  RainbowPlane::RainbowPlane(Services::ILedDriverAnimationContext& context):
    SpatialAnimation(context, "Rainbow Plane") {
    setBpm(bpm);
    updateRotation();
  }

  void RainbowPlane::getFields(Web::UI::FieldsInfo& fields) {
    fields.addSlider("speed", "Speed", 1, 60, "`${val} bpm`", 0);
    fields.addSlider("repeat", "Repeat", 1, 5*6,
      "switch(parseInt(val)){case 1:case 5:val+'/6';break;case 2:case 4:(val/2)+'/3';break;case 3:'1/2';break;default:Math.round(val/6);}");
    const char axis[3] = { 'X', 'Y', 'Z'};
    for(uint8_t i = 1; i < 3; i++) {
      String id = String("rotation") + String((int)i);
      String label = String(axis[i]) + String(" Rotation");
      fields.addSlider(id, label, 0, 360, "val+'&deg;'", 0);
    }
  }

  void RainbowPlane::getSettings(JsonObject& settings) const {
    settings["speed"] = bpm;
    settings["repeat"] = repeat6;
    for(uint8_t i = 1; i < 3; i++) {
      String id = String("rotation") + String((int)i);
      settings[id] = rotation[i]/CoordinateMax;
    }
  }

  void RainbowPlane::setSettings(const JsonObject& settings) {
    if (settings["speed"]) {
      setBpm(settings["speed"]);
    }
    if (settings["repeat"]) {
      repeat6 = settings["repeat"];
      if (repeat6 > 6) {
        repeat6 = round((float)repeat6/6)*6;
      }
    }
    bool setRotation = false;
    for(uint8_t i = 1; i < 3; i++) {
      String id = String("rotation") + String((int)i);
      if (!settings[id].isNull()) {
        rotation[i] = ((float)settings[id])*CoordinateMax;
        setRotation = true;
      }
    }
    
    if (setRotation) {
      updateRotation();
    }
  }

  void RainbowPlane::renderFrame() {
    EVERY_N_MILLIS_I(beat,everyMillis) {
      hue += hueIncrement;
    }
    Animation::renderFrame();
  }


  void RainbowPlane::renderLed(int index, const Coordinate& coordinate) {
    CoordinateType point[4] = {coordinate.x, coordinate.y, coordinate.z, 1};
    CoordinateType transformedPoint[4];
    rotationMatrix.multiplyVec(point, transformedPoint);

    byte offset = transformedPoint[0]*256*repeat6/CoordinateMax;
    context.hsv(hue+offset, 255, 255);
  }

  void RainbowPlane::setBpm(byte bpm) {
    this->bpm = bpm;
    hueIncrement = 1;
    float timeBetween = 234.375/bpm;
    // Don't update more than once every 20 milliseconds
    if (timeBetween < 20) {
      hueIncrement = 20/timeBetween;
      timeBetween *= hueIncrement;
    }
    everyMillis = timeBetween;
  }

  void RainbowPlane::updateRotation() {
    rotationMatrix = Algorithm::Matrix3D::rotation(rotation[0], rotation[1], rotation[2]);
  }
}}