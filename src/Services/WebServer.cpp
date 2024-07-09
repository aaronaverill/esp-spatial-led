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

namespace Services {
  WebServer *WebServer::instance = nullptr;

  void WebServer::addRequestHandler(const char* route, WebRequestMethodComposite method, ArRequestHandlerFunction requestHandler) {
    requestHandlers.push_back(Services::WebRequestHandler(route, method, requestHandler));
  }

  void WebServer::addRequestHandler(const char* route, WebRequestMethodComposite method, ArRequestHandlerFunction requestHandler, ArBodyHandlerFunction bodyHandler) {
    requestHandlers.push_back(Services::WebRequestHandler(route, method, requestHandler, bodyHandler));
  }

  void WebServer::setup() {
    dnsServer = new DNSServer();
    server = new AsyncWebServer(80);
    WiFi.softAP(ssid);
    dnsServer->start(53, "*", WiFi.softAPIP());

    for (Services::WebRequestHandler& handler:requestHandlers) {
      AsyncCallbackWebHandler* callbackHandler = new AsyncCallbackWebHandler();
      callbackHandler->setUri(handler.route);
      callbackHandler->setMethod(handler.method);
      callbackHandler->onRequest(handler.requestHandler);
      if (handler.hasBodyHandler) {
        Serial.println("set body handler");
        callbackHandler->onBody(handler.bodyHandler);
      }
      server->addHandler(callbackHandler);
    }
    requestHandlers.clear();
    
    server->addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
    server->begin();
  }

  void WebServer::loop() {
    dnsServer->processNextRequest();
  }
}