#pragma once
#include "Service.h"

#include <DNSServer.h>
#include "ESPAsyncWebServer.h"
#include "WebRequestHandler.h"

namespace Services {
  class WebServer: public Service {
    public:
      /**
       * Singleton accessor
       */
      static WebServer& getInstance() {
        if (instance == NULL) {
          instance = new WebServer();
        }
        return *instance;
      }

      /**
       * Add a request handler
       */
      void addRequestHandler(const char* route, WebRequestMethodComposite method, ArRequestHandlerFunction handler);

      /**
       * Methods for the arduino processing loop
       */
      void setup();
      void loop();
      
    private:
      WebServer() {}
      static WebServer *instance;

      String ssid = "SPATIAL-LED";

      DNSServer *dnsServer;
      AsyncWebServer *server;
      std::vector<Services::WebRequestHandler> requestHandlers;
  };
}