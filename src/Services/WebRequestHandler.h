#pragma once
#include <Arduino.h>
#include "ESPAsyncWebServer.h"

namespace Services {
  struct WebRequestHandler {
    WebRequestHandler(const char* route, WebRequestMethodComposite method, ArRequestHandlerFunction requestHandler): 
      route(route), method(method), hasRequestHandler(true), requestHandler(requestHandler) {}
    WebRequestHandler(const char* route, WebRequestMethodComposite method, ArBodyHandlerFunction bodyHandler)
      : route(route), method(method), hasBodyHandler(true), bodyHandler(bodyHandler) {}
    const char* route;
    WebRequestMethodComposite method;
    bool hasRequestHandler = false;
    ArRequestHandlerFunction requestHandler;
    bool hasBodyHandler = false;
    ArBodyHandlerFunction bodyHandler;
  };
}