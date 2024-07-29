#pragma once

#include <vector>
#include <Arduino.h>
#include <FastLED.h>

/**
 * A gradient palette
 */
class Palette {
  public:
    struct GradientStop {
      GradientStop(byte percent, byte r, byte g, byte b): percent(percent), rgb(r, g, b) {}

      byte percent;
      CRGB rgb;
    };

    Palette(const char *name): name(name) {}

    /**
     * Get the list of gradient stops
     */
    const std::vector<GradientStop>& getStops() const { return stops; }

    /**
     * Add a stop. If a stop at the specified percent exists it will be replaced
     */
    void addStop(byte percent, byte r, byte g, byte b) {
      size_t insertAt = 0;
      while (insertAt < stops.size() && percent > stops[insertAt].percent) {
        insertAt++;
      }
      GradientStop stop(percent, r, g, b);
      if (insertAt < stops.size()) {
        if (stops[insertAt].percent == percent) {
          stops[insertAt].rgb = stop.rgb;
        } else {
          stops.insert(stops.begin() + insertAt, stop);
        }
      } else {
        stops.push_back(stop);
      }
    }

    const char *name;

  private:
    std::vector<GradientStop> stops;
};