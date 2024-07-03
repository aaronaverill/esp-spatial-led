#pragma once
#include "Service.h"

#include <DNSServer.h>
#include "ESPAsyncWebServer.h"

namespace Services {
  class WebServer: public Service {
    public:
      WebServer() {}
      void setup();
      void loop();
    private:
      String ssid = "SPATIAL-LED";
      DNSServer *dnsServer;
      AsyncWebServer *server;
  };
}