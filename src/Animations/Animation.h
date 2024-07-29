#pragma once

#include <ArduinoJson.h>
#include "Services/ILedDriverAnimationContext.h"
#include "Web/UI/FieldsInfo.h"

namespace Animations {
  /**
   * Base class for all animations
   */
  class Animation {
    public:
      /**
       * Construct the animation with a context and name
       */
      Animation(Services::ILedDriverAnimationContext& context, const char *name, const char *tags = ""): 
        name(name), tags(tags), context(context) {}
      /**
       * Construct custom fields information
       */
      virtual void getFields(Web::UI::FieldsInfo& fields) {}
      /**
       * Construct custom settings
       */
      virtual void getSettings(JsonObject& settings) const {}
      /**
       * Set animation settings
       */
      virtual void setSettings(const JsonObject& settings) {}
      /**
       * Render the animation frame
       */
      virtual void renderFrame();
      /**
       * Render a specific led by index
       */
      virtual void renderLed(int index) {}

      const char *name;
      const char *tags;

    protected:
      Services::ILedDriverAnimationContext& context;
  };
}