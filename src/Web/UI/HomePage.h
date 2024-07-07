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
	.chevron-left {background-image:url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 12 12' stroke='white'%3E%3Cpath d='M8,2 4,6 8,10'/%3E%3C/svg%3E")}
	.chevron-right {background-image:url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 12 12' stroke='white'%3E%3Cpath d='M4,2 8,6 4,10'/%3E%3C/svg%3E")}
	.gear {background-image:url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 12 12' stroke='white' stroke-width='.75' fill='none'%3E%3Cpath d='M9.9,6.88 11.34,8 10.41,9.62 8.66,8.99 C8.23,9.37 7.73,9.65 7.19,9.82 L6.94,11.62 H5.06 L4.74,9.8 C4.2,9.62 3.7,9.33 3.29,8.94 L1.59,9.62 0.66,8 2.08,6.8 C1.96,6.25 1.97,5.68 2.09,5.12 L0.66,4 1.59,2.38 3.34,3.01 C3.77,2.63 4.27,2.35 4.81,2.18 L5.06,0.37 H6.94 L7.26,2.2 C7.8,2.38 8.3,2.67 8.71,3.06 L10.41,2.38 11.34,4 9.92,5.2 c0.11,0.56 0.11,1.13 -0.02,1.68z'/%3E%3Ccircle cx='6' cy='6' r='1.925'/%3E %3C/svg%3E")}
	.library {background-image:url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 12 12' stroke='white' fill='none'%3E%3Cpath d='m2.5,1 h7 m-8,2 h9'/%3E%3Crect x='0.5' y='5' width='11' height='6.5' rx='1' ry='1'/%3E%3C/svg%3E")}
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
			<div onclick="navClick(this)" data-page="1" class="selected"><div class="img lg library mb-2"></div><div>Library</div></div>
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
				clearTimeout(refreshTimer)
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