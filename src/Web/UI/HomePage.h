#pragma once
#include <Arduino.h>
#include "ESPAsyncWebServer.h"

namespace Web { namespace UI {
  class HomePage {
    public:
      static void get(AsyncWebServerRequest *request) {
        String html = R"(
<!DOCTYPE HTML>
<html>
<head>
<style>
  html{background-color:#000;color:#FFF;font-family:sans-serif;}
  .text-center{text-align:center}
</style>
</head>
<body onload='onload()'>
<h1 class='text-center'>ESP Spatial LED</h1>
<script>
async function onload() {
  let response = await fetch('/api/led/animations')
  if (response.ok) {
    const json = await response.json();
    console.log(json)
  } else {
    console.log(`Error: ${response.status}`)
  }
}
</script>
</body>
</html>)";
        AsyncResponseStream *response = request->beginResponseStream("text/html");
        response->print(html);
        request->send(response);
      } 
  };
}}