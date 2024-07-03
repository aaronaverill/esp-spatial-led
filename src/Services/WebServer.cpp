#include "Application.h"
#include "WebServer.h"
#include "LedDriver.h"

#include <DNSServer.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include "ESPAsyncWebServer.h"

namespace Services {
  class RequestHandler : public AsyncWebHandler {
    public:
      RequestHandler() {}
      virtual ~RequestHandler() {}

      bool canHandle(AsyncWebServerRequest *request){
        return true;
      }

      void handleRequest(AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("text/html");
        response->print(getPage());
        request->send(response);
      }

    private:
      String getPage() {
        String page = "<!DOCTYPE HTML>";
        page += "<html>";
        page += "<head>";
        page += "<style>";
        page += "html{background-color:#000;color:#FFF;}";
        page += "</style>";
        page += "</head>";
        page += "<body>";
        page += "<h1>ESP Spatial LED</h1>";
        page += "</body>";
        page += "</html>";
        return page;
      }
  };

  void WebServer::setup() {
    dnsServer = new DNSServer();
    server = new AsyncWebServer(80);
    WiFi.softAP(ssid);
    dnsServer->start(53, "*", WiFi.softAPIP());

    server->addHandler(new RequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
    server->begin();
  }

  void WebServer::loop() {
    dnsServer->processNextRequest();
  }
}