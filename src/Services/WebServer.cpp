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
  class CaptiveRequestHandler : public AsyncWebHandler {
    public:
      CaptiveRequestHandler() {}
      virtual ~CaptiveRequestHandler() {}

      bool canHandle(AsyncWebServerRequest *request){
        return true;
      }

      void handleRequest(AsyncWebServerRequest *request) {
        request->redirect("http://" + WiFi.softAPIP().toString());
      }
  };

  void WebServer::addRequestHandler(const char* route, WebRequestMethodComposite method, ArRequestHandlerFunction handler) {
    requestHandlers.push_back(Services::WebRequestHandler(route, method, handler));
  }

  void WebServer::setup() {
    dnsServer = new DNSServer();
    server = new AsyncWebServer(80);
    WiFi.softAP(ssid);
    dnsServer->start(53, "*", WiFi.softAPIP());

    for (Services::WebRequestHandler& handler:requestHandlers) {
      server->on(handler.route, handler.method, handler.handler);
    }
    requestHandlers.clear();
    
    server->addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
    server->begin();
  }

  void WebServer::loop() {
    dnsServer->processNextRequest();
  }
}