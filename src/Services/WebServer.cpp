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
  String getPage() {
    String page = "<!DOCTYPE HTML>";
    page += "<html>";
    page += "<head>";
    page += "<style>";
    page += "html{background-color:#000;color:#FFF;font-family:sans-serif;}";
    page += ".text-center{text-align:center}";
    page += "</style>";
    page += "</head>";
    page += "<body>";
    page += "<h1 class='text-center'>ESP Spatial LED</h1>";
    page += "</body>";
    page += "</html>";
    return page;
  }

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

  void WebServer::setup() {
    dnsServer = new DNSServer();
    server = new AsyncWebServer(80);
    WiFi.softAP(ssid);
    dnsServer->start(53, "*", WiFi.softAPIP());

    server->on("/", HTTP_GET, [] (AsyncWebServerRequest *request) {
      AsyncResponseStream *response = request->beginResponseStream("text/html");
      response->print(getPage());
      request->send(response);
    });
    server->addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
    server->begin();
  }

  void WebServer::loop() {
    dnsServer->processNextRequest();
  }

  void renderHome(AsyncWebServerRequest *request) {
    String html = R"(
<!DOCTYPE HTML>
<html>
<head>
<style>
  html{background-color:#000;color:#FFF;font-family:sans-serif;}
  .text-center{text-align:center}
</style>
</head>";
<body>
<h1 class='text-center'>ESP Spatial LED</h1>
</body>
</html>)";
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print(html);
    request->send(response);
  }
}