#pragma once
#include <Arduino.h>
#include "ESPAsyncWebServer.h"

namespace Web { namespace UI {
  class HomePage {
    public:
      static void get(AsyncWebServerRequest *request) {
        String html = R"rawstring(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width,initial-scale=1">
<style>
	html,body {height:100%}
	body {background:#000;color:#fff;font-family:sans-serif}
	.d-block {display:block}
	.d-flex {display:flex}
	.d-none {display:none}
	.flex-column {flex-direction:column}
	.flex-grow-1 {flex-grow:1}
	.w-100 {width:100%}
	.h-100 {height:100%}
	.ma-0 {margin:0}
	.mr-auto {margin-right:auto}
	.pa-3 {padding:12px}
	.pa-4 {padding:16px}
	.py-3 {padding-top:12px;padding-bottom:12px}
	.ml-3 {margin-left:12px}
	.mr-3 {margin-right:12px}
	.mb-2 {margin-bottom:8px}
	.text-center {text-align:center}
	.text-overflow {overflow:hidden;white-space:nowrap;text-overflow:ellipsis}
	.selectable>div:hover {background-color:#BBF2}
	.btn:hover {background-color:#FFF2}
	.list .item {display:flex;font-size:18px;align-items:center;padding:12px}
	.list .item.selected {background-color:#88F2}
	.list .item .text {overflow:hidden;white-space:nowrap;text-overflow:ellipsis;margin-right:auto}
	.main {min-height:10px;flex-grow:1}
	.title {display:flex;font-size:24px;align-items:center;background-color:indigo}
	.title>div {overflow:hidden;white-space:nowrap;text-overflow:ellipsis}
	.page {flex-grow:1;overflow-y:auto;background-color:#111}
	.play {flex-shrink:0;display:block}
	.nav {flex-shrink:0;background-color:indigo;text-align:center;cursor:pointer}
	.nav>div {display:flex;flex-direction:column;align-items:center;flex-grow:1;flex-basis:0;padding:12px;opacity:.6}
	.nav>div:hover {opacity:.9}
	.nav>div.selected {opacity:1}
	.img {width:18px;height:18px}
	.img.lg {width:24px;height:24px}
	.chevron-left {background-image:url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 16 16' fill='white'%3E%3Cpath d='M11.354 1.646a.5.5 0 0 1 0 .708L5.707 8l5.647 5.646a.5.5 0 0 1-.708.708l-6-6a.5.5 0 0 1 0-.708l6-6a.5.5 0 0 1 .708 0'/%3E%3C/svg%3E")}
	.chevron-right {background-image:url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 16 16' fill='white'%3E%3Cpath d='M4.646 1.646a.5.5 0 0 1 .708 0l6 6a.5.5 0 0 1 0 .708l-6 6a.5.5 0 0 1-.708-.708L10.293 8 4.646 2.354a.5.5 0 0 1 0-.708'/%3E%3C/svg%3E")}
	.collection {background-image:url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 16 16' fill='white'%3E%3Cpath d='M2.5 3.5a.5.5 0 0 1 0-1h11a.5.5 0 0 1 0 1zm2-2a.5.5 0 0 1 0-1h7a.5.5 0 0 1 0 1zM0 13a1.5 1.5 0 0 0 1.5 1.5h13A1.5 1.5 0 0 0 16 13V6a1.5 1.5 0 0 0-1.5-1.5h-13A1.5 1.5 0 0 0 0 6zm1.5.5A.5.5 0 0 1 1 13V6a.5.5 0 0 1 .5-.5h13a.5.5 0 0 1 .5.5v7a.5.5 0 0 1-.5.5z'/%3E%3C/svg%3E")}
	.gear {background-image:url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 16 16' fill='white'%3E%3Cpath d='M8 4.754a3.246 3.246 0 1 0 0 6.492 3.246 3.246 0 0 0 0-6.492M5.754 8a2.246 2.246 0 1 1 4.492 0 2.246 2.246 0 0 1-4.492 0'/%3E%3Cpath d='M9.796 1.343c-.527-1.79-3.065-1.79-3.592 0l-.094.319a.873.873 0 0 1-1.255.52l-.292-.16c-1.64-.892-3.433.902-2.54 2.541l.159.292a.873.873 0 0 1-.52 1.255l-.319.094c-1.79.527-1.79 3.065 0 3.592l.319.094a.873.873 0 0 1 .52 1.255l-.16.292c-.892 1.64.901 3.434 2.541 2.54l.292-.159a.873.873 0 0 1 1.255.52l.094.319c.527 1.79 3.065 1.79 3.592 0l.094-.319a.873.873 0 0 1 1.255-.52l.292.16c1.64.893 3.434-.902 2.54-2.541l-.159-.292a.873.873 0 0 1 .52-1.255l.319-.094c1.79-.527 1.79-3.065 0-3.592l-.319-.094a.873.873 0 0 1-.52-1.255l.16-.292c.893-1.64-.902-3.433-2.541-2.54l-.292.159a.873.873 0 0 1-1.255-.52zm-2.633.283c.246-.835 1.428-.835 1.674 0l.094.319a1.873 1.873 0 0 0 2.693 1.115l.291-.16c.764-.415 1.6.42 1.184 1.185l-.159.292a1.873 1.873 0 0 0 1.116 2.692l.318.094c.835.246.835 1.428 0 1.674l-.319.094a1.873 1.873 0 0 0-1.115 2.693l.16.291c.415.764-.42 1.6-1.185 1.184l-.291-.159a1.873 1.873 0 0 0-2.693 1.116l-.094.318c-.246.835-1.428.835-1.674 0l-.094-.319a1.873 1.873 0 0 0-2.692-1.115l-.292.16c-.764.415-1.6-.42-1.184-1.185l.159-.291A1.873 1.873 0 0 0 1.945 8.93l-.319-.094c-.835-.246-.835-1.428 0-1.674l.319-.094A1.873 1.873 0 0 0 3.06 4.377l-.16-.292c-.415-.764.42-1.6 1.185-1.184l.292.159a1.873 1.873 0 0 0 2.692-1.115z'/%3E%3C/svg%3E")}
</style>
</head>
<body class="ma-0" onload="onload()">
	<div class="d-flex flex-column h-100" class="h-100">
		<div class="main">
			<div id="p1" class="d-flex flex-column h-100">
				<div class="title"><div class="pa-3">Library</div></div>
				<div class="page">
					<div id="animations" class="list selectable"></div>
				</div>
			</div>
			<div id="p2" class="d-none flex-column h-100">
				<div class="title"><div class="pa-3">Settings</div></div>
				<div class="page">
					<div class="list selectable">
						<div onclick="showAbout()" class="item"><div class="text">About</div><div class="img chevron-right ml-3"></div></div>
					</div>
				</div>
			</div>
			<div id="p2.1" class="d-none flex-column h-100">
				<div class="title"><div onclick="showPage('2')" class="btn pa-4"><div class="img chevron-left"></div></div><div class="pa-3">About</div></div>
				<div class="page">
					<div id="fps" class="text-center pa-3">- fps</div>
				</div>
			</div>
		</div>
		<div class="play"><div id="play-name" class="pa-3">&nbsp;</div></div>
		<div class="nav d-flex selectable w-100">
			<div onclick="navClick(this)" data-page="1" class="selected"><div class="img lg collection mb-2"></div><div>Library</div></div>
			<div onclick="navClick(this)" data-page="2"><div class="img lg gear mb-2"></div><div>Settings</div></div>
		</div>
	</div>
	<script>
		let info = {
			leds: {
				animations:[]
			}
		}
		let refreshTimer=0
		async function onload() {
			let response = await fetch('/api/info')
			if (response.ok) {
				info = await response.json();
				refreshPage()
			} else {
				console.log(`Error: ${response.status}`)
			}
		}
		function onAnimationClick(i) {
			fetch(`/api/leds/play?index=${i}`,{method:'POST'})
			info.leds.play.index=i
			refreshPage()
		}
		function refreshPage() {
			if (info.leds.animations.length) {
				document.getElementById('play-name').innerText=info.leds.animations[info.leds.play.index].name
			}
			let html=''
			for(let i=0;i<info.leds.animations.length;i++) {
				const selected=i==info.leds.play.index?' selected':''
				html+='<div class="item'+selected+'" onclick="onAnimationClick('+i+')"><div class="text">'+info.leds.animations[i].name+'</div></div>'
			}
			document.getElementById('animations').innerHTML=html
		}
		function byClass(cls) {
			return document.querySelectorAll(cls)
		}
		function navClick(el) {
			showPage(el.dataset.page)
		}
		function showAbout() {
			showPage('2.1')
			updateFps()
		}
		function showPage(id) {
			if (refreshTimer) {
				clearTimeout(refreshTimer)
				refreshTimer=0
			}
			byClass('.nav>div').forEach(e=>{
				e.classList.remove('selected')
				if(e.dataset.page==id) e.classList.add('selected')
			})
			id='p'+id
			byClass('.main>div').forEach(e=>{
				e.classList.remove('d-flex','d-none')
				if(e.id==id) {
					e.classList.add('d-flex')
				} else {
					e.classList.add('d-none')
				}
			})
		}
		async function updateFps() {
			let response = await fetch('/api/leds/play/fps')
			if (response.ok) {
				let fps = await response.text();
				document.getElementById('fps').innerText=`${fps} fps`
				refreshTimer=setTimeout(updateFps,2000)
			} else {
				refreshTimer=0
			}
		}
	</script>
</body>
</html>
)rawstring";
        AsyncResponseStream *response = request->beginResponseStream("text/html");
        response->print(html);
        request->send(response);
      } 
  };
}}