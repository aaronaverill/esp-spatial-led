#pragma once
#include "LittleFS.h"
#include "IService.h"

namespace Services {
  class FileSystem: public IService {
    public:
      /**
       * Create singleton
       */
      static FileSystem& create() {
        if (instance == NULL) {
          instance = new FileSystem();
        }
        return *instance;
      }
      /**
       * Singleton accessor
       */
      static FileSystem& getInstance() { return *instance; }

      /**
       * Return true if the file system was mounted
       */
      const bool isMounted() { return mounted; }

      /**
       * Return true if the file exists
       */
      bool exists(const char *path) { return LittleFS.exists(path); }
      /**
       * Open a file
       */
      File open(const char *path, const char* mode) { return LittleFS.open(path, mode); }

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