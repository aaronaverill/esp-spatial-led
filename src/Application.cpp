#include <Arduino.h>
#include "Application.h"

Application *Application::instance = nullptr;

void Application::addService(Service* service) {
  services.push_back(service);
}

void Application::setup() {
  for (Service *service:services) {
    service->setup();
  }
}

void Application::loop() {
  for (Service *service:services) {
    service->loop();
  }
}