#pragma once
#include <Arduino.h>
#include "ESPAsyncWebServer.h"

namespace Services {
  struct WebRequestHandler {
    WebRequestHandler(const char* route, WebRequestMethodComposite method, ArRequestHandlerFunction requestHandler): 
      route(route), method(method), requestHandler(requestHandler) {}
    WebRequestHandler(const char* route, WebRequestMethodComposite method, ArRequestHandlerFunction requestHandler, ArBodyHandlerFunction bodyHandler)
      : route(route), method(method), requestHandler(requestHandler), hasBodyHandler(true), bodyHandler(bodyHandler) {}
    const char* route;
    WebRequestMethodComposite method;
    ArRequestHandlerFunction requestHandler;
    bool hasBodyHandler = false;
    ArBodyHandlerFunction bodyHandler;
  };
}