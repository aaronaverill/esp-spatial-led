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
      GradientStop(uint8_t percent, uint8_t r, uint8_t g, uint8_t b): percent(percent), rgb(r, g, b) {}

      uint8_t percent;
      CRGB rgb;
    };

    Palette(const String& name): name(name) {}

    /**
     * Get the list of gradient stops
     */
    const std::vector<GradientStop>& getStops() const { return stops; }
    /**
     * Set the list of gradient stops
     */
    void setStops(std::vector<GradientStop>& stops) { this->stops = stops; }

    /**
     * Add a stop. If a stop at the specified percent exists it will be replaced
     */
    void addStop(uint8_t percent, uint8_t r, uint8_t g, uint8_t b) {
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

    /**
     * Get the interpolated color at the specified percent of the palette gradient
     */
    CRGB getColor(uint8_t percent) const {
      // TODO: Replace the interpolation with integer math when passing a percent in as a uint8_t instead of a float
      return getColor((float)percent/255.f);
    }

    CRGB getColor(float percent) const {
      percent *= 255;
      if (stops.size() == 0) {
        return CRGB::Black;
      }
      uint8_t stopIndex = 0;
      while (stopIndex < stops.size() && stops[stopIndex].percent < percent) {
        stopIndex++;
      }
      // If the percent is past the last stop return the last stop
      if (stopIndex == stops.size()) {
        return stops[stopIndex-1].rgb;
      
      // If the percent is before the first stop or exactly on a stop, return it
      } else if (stopIndex == 0 || stops[stopIndex].percent == percent) {
        return stops[stopIndex].rgb;

      // Otherwise, interpolate between the points
      } else {
        const GradientStop& stopFrom = stops[stopIndex-1];
        const GradientStop& stopTo = stops[stopIndex];
        float range = stopTo.percent - stopFrom.percent;
        float percentOfRange = ((float)percent - stopFrom.percent) / range;
        return CRGB(
          stopTo.rgb.r == stopFrom.rgb.r || percentOfRange == 0 ? stopFrom.rgb.r : (float)stopFrom.rgb.r + ((float)stopTo.rgb.r - (float)stopFrom.rgb.r) * percentOfRange,
          stopTo.rgb.g == stopFrom.rgb.g || percentOfRange == 0 ? stopFrom.rgb.g : (float)stopFrom.rgb.g + ((float)stopTo.rgb.g - (float)stopFrom.rgb.g) * percentOfRange,
          stopTo.rgb.b == stopFrom.rgb.b || percentOfRange == 0 ? stopFrom.rgb.b : (float)stopFrom.rgb.b + ((float)stopTo.rgb.b - (float)stopFrom.rgb.b) * percentOfRange
        );
      }
    }

    String name;

  private:
    std::vector<GradientStop> stops;
};