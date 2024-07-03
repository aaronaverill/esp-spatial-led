#pragma once

#include "Services/ILedDriverAnimationContext.h"

namespace Animations {
  /**
   * Base class for all animations
   */
  class Animation {
    public:
      /**
       * Construct the animation with a context and name
       */
      Animation(Services::ILedDriverAnimationContext& context, const char *name) : context(context), name(name) {}
      /**
       * Render the animation frame
       */
      virtual void render();
      /**
       * Render a specific led by index
       */
      virtual void render(int index) {}

    protected:
      Services::ILedDriverAnimationContext& context;

    public:
      const char *name;
  };
}