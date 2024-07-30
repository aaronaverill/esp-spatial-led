#include "RainbowFade.h"

#include <FastLED.h>

namespace Animations {
  RainbowFade::RainbowFade(Services::ILedDriverAnimationContext& context): Animation(context, "Rainbow Fade") {
    beat.setBpm(bpm);
  }

  void RainbowFade::getFields(Web::UI::FieldsInfo& fields) {
    fields.addPaletteChooser("color", "Color");
    fields.addSlider("speed", "Speed", 1, 60, "`${val} bpm`", 0);
  }

  void RainbowFade::getSettings(JsonObject& settings) const {
    JsonObject jsonPalette = settings["color"].to<JsonObject>();
    palette.getSetting(jsonPalette);
    settings["speed"] = bpm;
  }

  void RainbowFade::setSettings(const JsonObject& settings) {
    if (settings["color"]) {
      palette.setSetting(settings["color"], context.getColors(), context.getPalettes());
    }
    if (settings["speed"]) {
      bpm = settings["speed"];
      beat.setBpm(bpm);
    }
  }

  void RainbowFade::renderFrame() {
    beat.tick();
    Animation::renderFrame();
  }

  void RainbowFade::renderLed(int index) {
    uint8_t percent = beat.value;
    CRGB rgb(palette.color.rgb);
    if (palette.number > 0 && palette.number <= context.getPalettes().size()) {
      rgb = context.getPalettes()[palette.number-1].getColor(percent);
    } else if (palette.color.number > 0 && palette.color.number <= context.getColors().size()) {
      rgb = context.getColors()[palette.color.number-1];
    }
    context.rgb(rgb.r, rgb.g, rgb.b);
  }
}