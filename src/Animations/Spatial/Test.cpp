#include "Test.h"

#include "System/Clock.h"
#include "Algorithm/Math.h"
#include "Algorithm/Quaternion.h"
#include "Algorithm/Waveform.h"

using namespace System;
using namespace Algorithm;

namespace Animations { namespace Spatial {
  Test::Test(Services::ILedDriverAnimationContext& context): SpatialAnimation(context, "Test") {
    randomSeed(millis());
    generateRandomVector();
  }

  void Test::getFields(Web::UI::FieldsInfo& fields) {
    fields.addSlider("speed", "Speed", 1, 4, nullptr, 0);
  }

  void Test::getSettings(JsonObject& settings) const {
    settings["speed"] = (60 / speed) / 65.535;
  }

  void Test::setSettings(const JsonObject& settings) {
    if (settings["speed"]) {
      speed = (60 / (float)settings["speed"]) / 65.535;
    }
  }

  void Test::renderFrame() {
    //EVERY_N_MILLISECONDS(20) {
    //  translationDistance += directionSpeed/5;
    //  if (fabsf(translationDistance) > 1.75) {
    //    generateRandomVector();
    //  }
    //}
    SpatialAnimation::renderFrame();
  }

  void Test::renderLed(int index, const Coordinate& coordinate) {
    Vector3 planeNormal = quaternion.getNormal();
    planeNormal.normalize();

    Vector3 position = Vector3(
      (float)coordinate.x / CoordinateMax - 0.5,
      (float)coordinate.y / CoordinateMax - 0.5,
      (float)coordinate.z / CoordinateMax - 0.5
    );

    float positionRelativeToPlane = planeNormal.dot(position) - translationDistance;

    float v = positionRelativeToPlane * 255;
    if (v < 0) {
      // TODO: Fix after merging with neopixelbus branch
      //context.rgb(0, 0, 0);
    } else {
      v = (byte)min(255.f, 255.f * v); 
      byte h = (byte)(128.f * positionRelativeToPlane);
              
      // TODO: Fix after merging with neopixelbus branch
      //context.hsv(h, 255, v);
    }
  }

  void Test::generateRandomVector() {
    directionSpeed = -directionSpeed;
    if (directionSpeed < 0) {
      float w = (float)rand() / RAND_MAX;
      float x = (float)rand() / RAND_MAX;
      float y = (float)rand() / RAND_MAX;
      float z = (float)rand() / RAND_MAX;
      
      quaternion = Quaternion(w, x, y, z);
      quaternion.normalize();
    }
  }
}}