#pragma once
#include <Arduino.h>
#include "ESPAsyncWebServer.h"

namespace Services {
  struct WebRequestHandler {
    WebRequestHandler(const char* route, WebRequestMethodComposite method, ArRequestHandlerFunction handler): route(route), method(method), handler(handler) {}
    const char* route;
    WebRequestMethodComposite method;
    ArRequestHandlerFunction handler;
  };
}