#include "Animation.h"

namespace Animations {
  /**
   * Default frame render loops through each led and delegates to the led render function
   */
  void Animation::renderFrame() {
    int currentLed = 0;
    int ledCount = context.getRenderLedCount();
    while (currentLed < ledCount) {
      context.setCurrentLed(currentLed);
      renderLed(currentLed);
      currentLed++;
    }
    FastLED.show();
  }
}