#pragma once
#include <Arduino.h>
#include "ESPAsyncWebServer.h"

namespace Web { namespace UI {
  class HomePage {
    public:
      static void get(AsyncWebServerRequest *request) {
        String html = R"rawstring(
<!DOCTYPE HTML>
<html>
<head>
<style>
  html{background-color:#000;color:#FFF;font-family:sans-serif;}
  .text-center{text-align:center}
  .flex{display:flex}
  .flex-row{flex-direction:row}
  .justify-center{justify-content:center}
  .ma-2{margin:8px}
  .ma-4{margin:16px}
  .px-4{padding:0 16px}
  div.selectable>div:hover{background-color:#222;cursor:pointer;}
  div.list>div{padding:12px;}
  div.list>div.selected{background-color:#555;}
</style>
</head>
<body onload="onload()">
<h1 class="text-center">ESP Spatial LED</h1>
<div class="flex flex-row justify-center">
  <button class="ma-4 px-4" onclick="onPrev()">Prev</button>
  <h2 id="current" class="text-center ma-4"></h2>
  <button class="ma-4 px-4" onclick="onNext()">Next</button>
</div>
<div id="animations" class="list selectable"></div>
<script>
let info = {animations:[]}
function onPrev() {
  if (!info.animations.length) return
  if (info.current > 0) {
    onAnimationClick(info.current - 1)
  } else {
    onAnimationClick(info.animations.length - 1)
  }
}
function onNext(){
  if (!info.animations.length) return
  if (info.current < info.animations.length - 1) {
    onAnimationClick(info.current + 1)
  } else {
    onAnimationClick(0)
  }
}
function onAnimationClick(i){
  fetch(`/api/leds/current?index=${i}`,{method:'POST'})
  info.current = i
  refreshPage()
}
function refreshPage(){
  if (info.animations.length) {
    document.getElementById('current').innerText = info.animations[info.current].name
  }
  var html = '';
  for(let i = 0; i < info.animations.length; i++) {
    const selected = i == info.current ? " class='selected'" : ""
    html += '<div'+selected+' onclick="onAnimationClick('+i+')">'+info.animations[i].name+'</div>'
  }
  document.getElementById('animations').innerHTML = html;
}
async function onload(){
  let response = await fetch('/api/leds')
  if (response.ok) {
    info = await response.json();
    refreshPage();
  } else {
    console.log(`Error: ${response.status}`)
  }
}
</script>
</body>
</html>)rawstring";
        AsyncResponseStream *response = request->beginResponseStream("text/html");
        response->print(html);
        request->send(response);
      } 
  };
}}