#include "RainbowChase.h"

#include <FastLED.h>

namespace Animations {
  RainbowChase::RainbowChase(Services::ILedDriverAnimationContext& context): Animation(context, "Rainbow Chase") {
    beat.setBpm(bpm);
  }

  void RainbowChase::getFields(Web::UI::FieldsInfo& fields) {
    fields.addPaletteChooser("color", "Color");
    fields.addSlider("repeat", "Repeat", 1, 5*6,
      "switch(parseInt(val)){case 1:case 5:val+'/6';break;case 2:case 4:(val/2)+'/3';break;case 3:'1/2';break;default:Math.round(val/6);}");
    fields.addSlider("speed", "Speed", 1, 60, "`${val} bpm`", 0);
  }

  void RainbowChase::getSettings(JsonObject& settings) const {
    JsonObject jsonPalette = settings["color"].to<JsonObject>();
    palette.getSetting(jsonPalette);
    settings["repeat"] = repeat6;
    settings["speed"] = bpm;
  }

  void RainbowChase::setSettings(const JsonObject& settings) {
    if (settings["color"]) {
      palette.setSetting(settings["color"], context.getColors(), context.getPalettes());
    }
    if (settings["repeat"]) {
      repeat6 = settings["repeat"];
      if (repeat6 > 6) {
        repeat6 = round((float)repeat6/6)*6;
      }
    }
    if (settings["speed"]) {
      bpm = settings["speed"];
      beat.setBpm(bpm);
    }
  }

  void RainbowChase::renderFrame() {
    beat.tick();
    Animation::renderFrame();
  }

  void RainbowChase::renderLed(int index) {
    uint8_t offset = (float)index*256/context.getRenderLedCount()*repeat6/6;
    uint8_t percent = beat.value+offset;
    CRGB rgb(palette.color.rgb);
    if (palette.number > 0 && palette.number <= context.getPalettes().size()) {
      rgb = context.getPalettes()[palette.number-1].getColor(percent);
    } else if (palette.color.number > 0 && palette.color.number <= context.getColors().size()) {
      rgb = context.getColors()[palette.color.number-1];
    }
    context.rgb(rgb.r, rgb.g, rgb.b);
  }
}