#pragma once
#include "IService.h"

namespace Services {
  class FileSystem: public IService {
    public:
      /**
       * Singleton accessor
       */
      static FileSystem& getInstance() {
        if (instance == NULL) {
          instance = new FileSystem();
        }
        return *instance;
      }

      /**
       * Return true if the file system was mounted
       */
      const bool isMounted() { return mounted; }

      /**
       * Methods for the arduino processing loop
       */
      void setup();
      void loop();

    private:
      bool mounted = false;
      
      FileSystem();
      static FileSystem *instance;
  };
}