#pragma once
#include "IService.h"

#include <DNSServer.h>
#include "ESPAsyncWebServer.h"
#include "WebRequestHandler.h"

namespace Services {
  /**
   * A service that handles HTTP requests.
   */
  class WebServer: public IService {
    public:
      /**
       * Create singleton
       */
      static WebServer& create() {
        if (instance == NULL) {
          instance = new WebServer();
        }
        return *instance;
      }
      /**
       * Singleton accessor
       */
      static WebServer& getInstance() { return *instance; }

      /**
       * Add a request handler
       */
      void addRequestHandler(const char* route, WebRequestMethodComposite method, ArRequestHandlerFunction requestHandler);
      void addRequestHandler(const char* route, WebRequestMethodComposite method, ArRequestHandlerFunction requestHandler, ArBodyHandlerFunction bodyHandler);

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