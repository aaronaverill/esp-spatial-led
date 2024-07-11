#pragma once
#include <Arduino.h>
#include "ESPAsyncWebServer.h"

#include "Content/index_html.h"
#include "Content/app_css.h"
#include "Content/app_js.h"

namespace Web { namespace UI {
  void sendGZipResponse(AsyncWebServerRequest *request, const char *contentType, const uint8_t *content, size_t length) {
    AsyncWebServerResponse *response = request->beginResponse_P(200, contentType, content, length);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  }

  /**
   * HTTP Request handler class for website requests.
   * Returns an HTML page that is a single page application (SPA) which controls the software.
   */
  class HomePage {
    public:
      static void get(AsyncWebServerRequest *request) {
        Serial.println("Web::Homepage::get " + request->url());

        if (request->url().equals("/connecttest.txt")) {
          request->send(200, "text/plain", "Microsoft Connect Test");
          return;
        } else if (request->url().equals("/redirect")) {
          request->redirect("/");
          return;
        }

        if (request->url().equals("/")) {
          sendGZipResponse(request, "text/html", index_html, index_html_length);
          return;
        } else if (request->url().equals("/app.css")) {
          sendGZipResponse(request, "text/css", app_css, app_css_length);
          return;
        } else if (request->url().equals("/app.js")) {
          sendGZipResponse(request, "text/javascript", app_js, app_js_length);
          return;
        }
        
        Serial.println("Page Not Found: " + request->url());
        request->send(404);
      } 
  };
}}