#pragma once
#include "Service.h"

#include <DNSServer.h>
#include "ESPAsyncWebServer.h"
#include "WebRequestHandler.h"

namespace Services {
  class WebServer: public Service {
    public:
      WebServer() {}
      void addRequestHandler(const char* route, WebRequestMethodComposite method, ArRequestHandlerFunction handler);
      AsyncWebServer* getWebServer() { return server; }

      void setup();
      void loop();
    private:
      String ssid = "SPATIAL-LED";
      DNSServer *dnsServer;
      AsyncWebServer *server;

      std::vector<Services::WebRequestHandler> requestHandlers;
  };
}