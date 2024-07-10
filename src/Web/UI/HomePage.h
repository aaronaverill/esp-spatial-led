#pragma once
#include <Arduino.h>
#include "ESPAsyncWebServer.h"

const char index_html[] PROGMEM = R"rawstring(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width,initial-scale=1">
<style>
	html,body {height:100%}
	body {background:#000;color:#fff;font-family:sans-serif}
	select, input {color:white;background-color:#222;padding:8px;box-sizing:border-box;border:1px solid #444;border-radius:3px}
	input::-webkit-outer-spin-button, input::-webkit-inner-spin-button {-webkit-appearance:none}
	.font-micro {font-size:70%}
	.d-block {display:block}
	.d-flex {display:flex}
	.d-none {display:none}
	.flex-column {flex-direction:column}
	.flex-grow-1 {flex-grow:1}
	.flex-shrink-0 {flex-shrink:0}
	.w-100 {width:100%}
	.h-100 {height:100%}
	.ma-0 {margin:0}
	.mr-auto {margin-right:auto}
	.ma-2 {margin:8px}
	.my-2 {margin-top:8px;margin-bottom:8px}
	.ml-3 {margin-left:12px}
	.mr-3 {margin-right:12px}
	.mb-2 {margin-bottom:8px}
	.pa-2 {padding:8px}
	.pa-3 {padding:12px}
	.pa-4 {padding:16px}
	.py-3 {padding-top:12px;padding-bottom:12px}
	.pr-3 {padding-right:12px}
	.rounded {border-radius:6px}
	.text-center {text-align:center}
	.text-truncate {overflow:hidden;white-space:nowrap;text-overflow:ellipsis}
	.align-center {align-items:center}
	.justify-between {justify-content:space-between}
	.selectable,.btn {cursor:pointer}
	.selectable>div:hover {background-color:#BBF2}
	.btn.tonal {font-size:16px;padding:5px 12px 8px;font-variant:all-small-caps;border-radius:4px;letter-spacing:.7px}
	.btn:hover {background-color:#FFF2}
	.btn.primary {background-color:#04AA6D}
	.btn.primary:hover {background-color:#05B571}
	.list .item {font-size:18px}
	.list .item.selected {background-color:#88F2}
	.list .item .text {overflow:hidden;white-space:nowrap;text-overflow:ellipsis;margin-right:auto}
	.main {min-height:10px;flex-grow:1}
	.title {display:flex;font-size:24px;align-items:center;background-color:#282A35}
	.title>div {overflow:hidden;white-space:nowrap;text-overflow:ellipsis}
	.page {flex-grow:1;overflow-y:auto;background-color:#111}
	.play {flex-shrink:0;display:flex;align-items:center}
	.nav {flex-shrink:0;background-color:#282A35;text-align:center;cursor:pointer}
	.nav>div {display:flex;flex-direction:column;align-items:center;flex-grow:1;flex-basis:0;padding:12px;opacity:.6}
	.nav>div:hover {opacity:.9}
	.nav>div.selected {opacity:1}
	.options .value {width:4em;text-align:right}
	.rainbow {background-image:linear-gradient(90deg,#F00,#FF0,#0F0,#0FF,#00F,#F0F,#F00)}
	.img {width:18px;height:18px}
	.img.img-lg {width:24px;height:24px}
	.img.img-vlg {width:34px;height:34px}
	.img-led {background-image:url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' stroke='white' fill='none' stroke-linecap='round'%3E%3Cpath stroke-linejoin='round' d='m10,21.28 v-4.18 m0,-3 V9.63 h1.04 l0.53,0.85 1.06,-0.85 h1.26 v4.47 m0,3 V23.46 M6.75,14.1 H17.25 c0.55,0 1,0.45 1,1 v1 c0,0.55 -0.44,1 -1,1 H6.75 c-0.55,0 -1,-0.45 -1,-1 v-1 c0,-0.55 0.45,-1 1,-1 zM16.75,13.8 V5.25 C16.75,2.63 14.62,0.5 12,0.5 c-2.62,0 -4.75,2.13 -4.75,4.75 v 8.55'/%3E%3Cpath d='M8.82,5.25 A3.18,3.18 0 0 1 12,2.09'/%3E%3C/svg%3E")}
	.img-arrow-back {background-image:url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 12 12' stroke='white' fill='none'%3E%3Cpath d='M2,6 H 9.5 M5.5,2.5 2,6 5.5,9.5'/%3E%3C/svg%3E")}
	.img-chevron-right {background-image:url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 12 12' stroke='white' fill='none'%3E%3Cpath d='M4,2 8,6 4,10'/%3E%3C/svg%3E")}
	.img-close {background-image:url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 12 12' stroke='white' fill='none'%3E%3Cpath d='m2,10 8,-8 m-8,0 8,8'/%3E%3C/svg%3E")}
	.img-gear {background-image:url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 12 12' stroke='white' stroke-width='.75' fill='none'%3E%3Cpath d='M9.9,6.88 11.34,8 10.41,9.62 8.66,8.99 C8.23,9.37 7.73,9.65 7.19,9.82 L6.94,11.62 H5.06 L4.74,9.8 C4.2,9.62 3.7,9.33 3.29,8.94 L1.59,9.62 0.66,8 2.08,6.8 C1.96,6.25 1.97,5.68 2.09,5.12 L0.66,4 1.59,2.38 3.34,3.01 C3.77,2.63 4.27,2.35 4.81,2.18 L5.06,0.37 H6.94 L7.26,2.2 C7.8,2.38 8.3,2.67 8.71,3.06 L10.41,2.38 11.34,4 9.92,5.2 c0.11,0.56 0.11,1.13 -0.02,1.68z'/%3E%3Ccircle cx='6' cy='6' r='1.925'/%3E %3C/svg%3E")}
	.img-options {background-image:url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 12 12' stroke='white' fill='none'%3E%3Cpath d='m6,8.5 v3 M6,10 h5.5 m-11,0 h3 m0,-5.5 v3 M3.5,6 h-3 m11,0 H6 M8.5,0.5 v3 M8.5,2 h3 m-11,0 h5.5'/%3E%3C/svg%3E")}
	.img-library {background-image:url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 12 12' stroke='white' fill='none'%3E%3Cpath d='m2.5,1 h7 m-8,2 h9'/%3E%3Crect x='0.5' y='5' width='11' height='6.5' rx='1' ry='1'/%3E%3C/svg%3E")}
</style>
</head>
<body class="ma-0" onload="onload()">
	<div class="d-flex flex-column h-100" class="h-100">
		<div class="main">
			<div id="p1" class="d-flex flex-column h-100">
				<div class="title"><div class="img img-vlg img-led ma-2"></div><div class="pa-3">Library</div></div>
				<div class="page">
					<div id="animations" class="list selectable"></div>
				</div>
			</div>
			<div id="p2" class="d-none flex-column h-100">
				<div class="title"><div class="img img-vlg img-led ma-2"></div><div class="pa-3">Settings</div></div>
				<div class="page">
					<div class="list selectable">
						<div onclick="showPage('2.1')" class="item d-flex align-center pa-3"><div class="text mr-3">LED Layout</div><div class="img img-chevron-right flex-shrink-0"></div></div>
						<div onclick="showAbout()" class="item d-flex align-center pa-3"><div class="text mr-3">About</div><div class="img img-chevron-right flex-shrink-0"></div></div>
					</div>
				</div>
			</div>
			<div id="p1.1" class="d-none flex-column h-100">
				<div class="title"><div onclick="showPage('1')" class="btn pa-4"><div class="img img-close"></div></div><div class="pa-3">Options</div></div>
				<div class="page options">
					<div class="list">
						<div data-field="brightness" class="item pa-2">
							<div>Brightness</div>
							<div class="d-flex w-100">
								<div class="flex-grow-1 pr-3">
									<div><input type="range" oninput="onBrightnessChange(this)" min="0" max="100" class="w-100"></input></div>
									<div class="d-flex justify-between font-micro">
										<div>0%</div>
										<div>100%</div>
									</div>
								</div>
								<div class="value flex-shrink-0"></div>
							</div>
						</div>
					</div>
				</div>
			</div>
			<div id="p2.1" class="d-none flex-column h-100">
				<div class="title">
					<div onclick="showPage('2')" class="btn pa-4"><div class="img img-arrow-back"></div></div>
					<div class="pa-3 mr-auto">LED Layout</div>
					<div class="mr-3"><div onclick="onSaveLedLayout()" class="btn tonal primary">Save</div></div>
				</div>
				<div class="page pa-3">
					<div>Layout</div>
					<select id="ledLayoutType" class="w-100 my-2">
						<option value="strip">Strip</option>
					</select>
					<div>Number of LEDs</div>
					<input id="ledCount" type="number" onkeypress="posIntKey(event)" class="w-100 my-2" />
				</div>
			</div>
			<div id="p2.2" class="d-none flex-column h-100">
				<div class="title"><div onclick="showPage('2')" class="btn pa-4"><div class="img img-arrow-back"></div></div><div class="pa-3">About</div></div>
				<div class="page">
					<div id="fps" class="text-center pa-3"></div>
				</div>
			</div>
		</div>
		<div class="play">
			<div id="play-name" class="text-truncate pa-3 mr-auto">&nbsp;</div>
			<div onclick="showOptions()" class="btn pa-3"><div class="img img-options"></div></div>
		</div>
		<div class="nav d-flex selectable w-100">
			<div onclick="navClick(this)" data-page="1" class="selected"><div class="img img-lg img-library mb-2"></div><div>Library</div></div>
			<div onclick="navClick(this)" data-page="2"><div class="img img-lg img-gear mb-2"></div><div>Settings</div></div>
		</div>
	</div>
	<script>
		let globalFields = [
			{
				id: 'brightness',
				type: 'slider',
				label: 'Brightness',
				min: 0,
				max: 100,
				factor: 2.55,
				template: '${val}%'
			}
		]
		let info = {
			leds: {
				count: 1,
				coordinates: {config:'{}',values:''},
				play: {
					settings: {
						brightness: 128
					}
				},
				animations: []
			}
		}
		let refreshTimer = 0

		async function onload() {
			let response = await fetch('/api/info')
			if (response.ok) {
				info = await response.json()
				refreshPage()
			} else {
				console.log(`Error: ${response.status}`)
			}
		}
		function onAnimationClick(i) {
			fetch(`/api/leds/play?index=${i}`, {method:'POST'})
			info.leds.play.index = i
			refreshPage()
		}
		function refreshPage() {
			if (info.leds.animations.length) {
				byId('play-name').innerText = info.leds.animations[info.leds.play.index].name
			}
			let html=''
			for(let i = 0; i < info.leds.animations.length; i++) {
				let selected = i == info.leds.play.index ? ' selected' : ''
				html += '<div class="item pa-3' + selected + '" onclick="onAnimationClick(' + i + ')"><div class="text">' + info.leds.animations[i].name + '</div></div>'
			}
			byId('animations').innerHTML=html
			refreshLedLayout()
		}
		function refreshLedLayout() {
			byId('ledCount').value=info.leds.count
		}
		function navClick(el) {
			showPage(el.dataset.page)
		}
		function showAbout() {
			showPage('2.2')
			updateFps()
		}
		function showOptions() {
			let a = info.leds.animations[info.leds.play.index]
			let list = document.querySelector('.page.options .list')
			let html = ''
			let fields = []
			for (let f of globalFields) fields.push(f.id)
			if (a.fields) {
				for(let f of a.fields) fields.push(f.id)
			}
			for (let f of fields) {
				html+=optionHtml(optionsField(f))
			}
			list.innerHTML=html
			for (let f of fields) {
				refreshOptionControls(optionsField(f))
			}
			showPage('1.1')
		}
		function showPage(id) {
			if (refreshTimer) {
				clearTimeout(refreshTimer)
				refreshTimer = 0
			}
			queryAll('.nav>div').forEach(e => {
				e.classList.remove('selected')
				if (e.dataset.page==id) e.classList.add('selected')
			})
			id='p'+id
			queryAll('.main>div').forEach(e => {
				e.classList.remove('d-flex','d-none')
				e.classList.add(e.id == id ? 'd-flex' : 'd-none')
			})
		}
		function optionHtml(field) {
			var html = `<div data-field="${field.id}" class="item pa-2"><div>${field.label}</div><div class="d-flex w-100"><div class="flex-grow-1 pr-3">`

			switch (field.type) {
				case'slider':
					let val = field.min
					let minLabel = field.template ? eval(`\`${field.template}\``) : val
					val = field.max
					let maxLabel = field.template ? eval(`\`${field.template}\``) : val
					html += `
<div><input type="range" oninput="onInputChange('${field.id}',this)" min="${field.min}" max="${field.max}" class="w-100"></input></div>
<div class="d-flex justify-between font-micro">
	<div>${minLabel}</div>
	<div>${maxLabel}</div>
</div>
</div><div class="value flex-shrink-0"></div>`;
					break
				case'hue-slider':
					html += `<div><input type="range" oninput="onInputChange('${field.id}',this)" min="${field.min}" max="${field.max}" class="w-100"></input></div>`
					html += '<div class="rainbow" style="height:10px;margin:0 5px 0 9px"></div>'
					html += '</div><div class="value flex-shrink-0 rounded"></div>'
					break
			}
			html+='</div></div>';
		
			return html
		}
		function onInputChange(id,e) {
			let field = optionsField(id)
			let val = e.value
			let modelVal = Math.round(modelValue(val,field))
			if (isGlobalField(id)) {
				info.leds.play.settings[id] = modelVal
			} else {
				info.leds.animations[info.leds.play.index].settings[id] = modelVal
			}
			refreshOptionValue(e,val,field)
			if (isGlobalField(id)) {
				fetch(`/api/leds/play/settings?${id}=${modelVal}`,{method:'POST'})
			} else {
				fetch(`/api/leds/animations/settings?index=${info.leds.play.index}&${id}=${modelVal}`,{method:'POST'})
			}
		}
		function isGlobalField(id) {
			return globalFields.some(f => f.id == id)
		}
		function optionsField(id) {
			return globalFields.find(f => f.id == id)|| info.leds.animations[info.leds.play.index].fields?.find( f=> f.id == id)
		}
		function modelValue(val,field) {
			if (field.factor) val *= field.factor
			return val
		}
		function refreshOptionControls(field) {
			let input = document.querySelector(`.page.options [data-field="${field.id}"] input`)
			let val = info.leds.play.settings[field.id] || info.leds.animations[info.leds.play.index].settings[field.id]
			if (field.factor) val /= field.factor
			input.value = val
			val = Math.round(val)
			refreshOptionValue(input,val,field)
		}
		function refreshOptionValue(e,val,field) {
			switch (field.type) {
				case 'slider':
					if (field.template) val = eval(`\`${field.template}\``)
					e.closest('.item').querySelector('.value').innerText = val
					break
				case 'hue-slider':
					val *= (360/255)
					e.closest('.item').querySelector('.value').style.backgroundColor=`hsl(${val},100%,50%)`
					break
			}
		}
		async function updateFps() {
			let response = await fetch('/api/leds/play/fps')
			if (response.ok) {
				let fps = await response.text()
				byId('fps').innerText = `${fps} fps`
				refreshTimer = setTimeout(updateFps,2000)
			} else {
				clearTimeout(refreshTimer)
				refreshTimer = 0
			}
		}
		async function onSaveLedLayout() {
			// TODO: Check if valid
			var settings = null
			switch (byId('ledLayoutType').value) {
				case 'strip':
					let ledCount = byId('ledCount').value
					let values = ''
					for (let i = 0; i < ledCount; i++) {
						if (i > 0) values += ','
						values += Math.round(10000*(i+.5)/ledCount)/10000 + ',0,0'
					}
					settings = {
						ledCount: ledCount,
						coordinates: {
							config: '{"layout":"strip"}',
							values: values
						}
					}
					break
			}
			if (settings) {
				await fetch('/api/leds/settings',{
					method:'PATCH',
					body:JSON.stringify(settings)
				})
			}
		}
		function queryAll(cls) { return document.querySelectorAll(cls) }
		function byId(id) { return document.getElementById(id) }
		function posIntKey(e) { if(e.key === "." || e.key === "-") e.preventDefault() }
	</script>
</body>
</html>
)rawstring";

namespace Web { namespace UI {
  /**
   * HTTP Request handler class for website requests.
   * Returns an HTML page that is a single page application (SPA) which controls the software.
   */
  class HomePage {
    public:
      static void get(AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response;
        response = request->beginResponse_P(200, "text/html", index_html);
        request->send(response);
      } 
  };
}}