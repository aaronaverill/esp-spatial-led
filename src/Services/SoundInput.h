#pragma once
#include <FS.h>
#include <arduinoFFT.h>

#include "Services/IService.h"

namespace Services {
  /**
   * A service that manages microphone input
   */
  class SoundInput: public IService {
    public:
      /**
       * Create singleton
       */
      static SoundInput& create(FS& fs) {
        if (instance == NULL) {
          instance = new SoundInput(fs);
        }
        return *instance;
      }
      /**
       * Methods for the arduino processing loop
       */
      void setup();
      void loop();

      float* getVolumeByFrequency();
    private:
      void readI2S();
      void computeFFT();
      
      SoundInput(FS& fs);
      static SoundInput *instance;
      FS& fs;
  };
}