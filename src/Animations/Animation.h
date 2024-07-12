#pragma once

#include <ArduinoJson.h>
#include "KeyValuePair.h"
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
      Animation(Services::ILedDriverAnimationContext& context, const char *name) : context(context), name(name) {}
      /**
       * Construct custom fields information
       */
      virtual void getFields(Web::UI::FieldsInfo& fields) {}
      /**
       * Construct custom settings
       */
      virtual void getSettings(JsonObject& settings) {}
      /**
       * Set animation settings
       */
      virtual void setSettings(std::vector<KeyValuePair<String, String>> values) {}
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