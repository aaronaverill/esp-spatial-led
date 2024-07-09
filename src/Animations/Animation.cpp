#include "Animation.h"

namespace Animations {
  /**
   * Default frame render loops through each led and delegates to the led render function
   */
  void Animation::render() {
    int currentLed = 0;
    int ledCount = context.getRenderLedCount();
    while (currentLed < ledCount) {
      context.setCurrentLed(currentLed);
      render(currentLed);
      currentLed++;
    }
    FastLED.show();
  }
}