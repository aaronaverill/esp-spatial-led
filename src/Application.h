#pragma once
#include <vector>

#include "Services/IService.h"

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
      return instance;
    }

    /**
     * Add a service to the arduino setup() and loop() pipeline
     */
    void addService(IService* service);

    /**
     * Called from main.cpp setup()
     */
    void setup();
    /**
     * Called from main.cpp loop()
     */
    void loop();

  private:
    Application();
    static Application instance;
    std::vector<IService*> services;
};
