#pragma once
#include "Services/Service.h"

#include <vector>

using namespace Services;

/**
 * Application class orchestrates the arduino pipeline by delegating to services
 */
class Application {
  public:
    /**
     * Singleton accessor
     */
    static Application& getInstance() {
      if (instance == nullptr) {
        instance = new Application();
      }
      return *instance;
    }

    /**
     * Add a service to the arduino setup() and loop() pipeline
     */
    void addService(Service* service);

    /**
     * Called from main.cpp setup()
     */
    void setup();
    /**
     * Called from main.cpp loop()
     */
    void loop();

  private:
    Application() {}
    static Application *instance;
    std::vector<Service*> services;
};
