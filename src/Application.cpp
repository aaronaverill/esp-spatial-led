#include <Arduino.h>
#include "Application.h"

Application *Application::instance = nullptr;

void Application::addService(IService* service) {
  services.push_back(service);
}

void Application::setup() {
  for (IService *service:services) {
    service->setup();
  }
}

void Application::loop() {
  for (IService *service:services) {
    service->loop();
  }
}