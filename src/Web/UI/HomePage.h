#pragma once
#include <Arduino.h>
#include "ESPAsyncWebServer.h"

#include "Content/index_html.h"
#include "Content/app_css.h"
#include "Content/app_js.h"

namespace Web { namespace UI {
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

        AsyncWebServerResponse *response;
        const char *contentType = "text/html";
        const char *content = nullptr;
        if (request->url().equals("/")) {
          content = index_html;
        } else if (request->url().equals("/app.css")) {
          contentType = "text/css";
          content = app_css;
        } else if (request->url().equals("/app.js")) {
          contentType = "text/javascript";
          content = app_js;
        }
        if (content != nullptr) {
          response = request->beginResponse_P(200, contentType, content);
          request->send(response);
        } else {
          Serial.println("Page Not Found: " + request->url());
          request->send(404);
        }
      } 
  };
}}