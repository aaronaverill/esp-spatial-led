#include <Arduino.h>
#include "Application.h"

Application *Application::instance = nullptr;

void Application::addService(Service* service) {
  services.push_back(service);
}

const Service* Application::getServiceByName(const char *name) {
  for (Service *service:services) {
    if (strcmp(service->name, name) == 0) {
      return service;
    }
  }
  return nullptr;
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