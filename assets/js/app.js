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
let info
let refreshTimer

// Utility functions
function query(s) { return document.querySelector(s) }
function queryAll(s) { return document.querySelectorAll(s) }
function byId(i) { return document.getElementById(i) }
function posIntKey(e) { if(e.key === "." || e.key === "-") e.preventDefault() }

// App load
async function onload() {
  try {
    let response = await fetch('/api/info')
    if (!response.ok) throw `Error: ${response.status}`
    info = await response.json()
  } catch (e) {
    console.log(e)
  }
  refreshUI()
}

// App
function refreshUI() {
  if (info.leds.animations.length) {
    byId('play-name').innerText = info.leds.animations[info.leds.play.index].name
  }
  refreshLibrary()
  refreshLedLayout()
}

function showPage(id) {
  if (refreshTimer) {
    clearTimeout(refreshTimer)
    refreshTimer = 0
  }
  queryAll('.nav>div').forEach(e => {
    e.classList.remove('selected')
    if (e.dataset.page == id) e.classList.add('selected')
  })
  queryAll('.main>div').forEach(e => {
    e.classList.remove('d-flex','d-none')
    e.classList.add(e.id == id ? 'd-flex' : 'd-none')
  })
}

// Play options page
function optionsField(id) {
  return globalFields.find(f => f.id == id)|| info.leds.animations[info.leds.play.index].fields?.find( f=> f.id == id)
}

function showOptions() {
  let a = info.leds.animations[info.leds.play.index]
  let list = query('.page.options .list')
  let html = ''
  let fields = []
  for (let f of globalFields) fields.push(f.id)
  if (a.fields) {
    for(let f of a.fields) fields.push(f.id)
  }
  for (let f of fields) {
    html += optionHtml(optionsField(f))
  }
  list.innerHTML = html
  for (let f of fields) {
    refreshOptionControls(optionsField(f))
  }
  showPage('pPlayOptions')
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

function refreshOptionControls(field) {
  let input = query(`.page.options [data-field="${field.id}"] input`)
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

function onInputChange(id,e) {
  let field = optionsField(id)
  let isGlobal = globalFields.some(f => f.id == id)
  let val = e.value
  let modelVal = val
  if (field.factor) modelVal *= field.factor
  modelVal = Math.round(modelVal)
  if (isGlobal) {
    info.leds.play.settings[id] = modelVal
  } else {
    info.leds.animations[info.leds.play.index].settings[id] = modelVal
  }
  refreshOptionValue(e,val,field)
  if (isGlobal) {
    fetch(`/api/leds/play/settings?${id}=${modelVal}`,{method:'POST'})
  } else {
    fetch(`/api/leds/animations/settings?index=${info.leds.play.index}&${id}=${modelVal}`,{method:'POST'})
  }
}

// Library page
function refreshLibrary() {
  let html = ''
  for(let i = 0; i < info.leds.animations.length; i++) {
    let selected = i == info.leds.play.index ? ' selected' : ''
    html += '<div class="item pa-3' + selected + '" onclick="onAnimationClick(' + i + ')"><div class="text">' + info.leds.animations[i].name + '</div></div>'
  }
  byId('animations').innerHTML = html
}

function onAnimationClick(i) {
  fetch(`/api/leds/play?index=${i}`, {method:'POST'})
  info.leds.play.index = i
  refreshUI()
}

// Led Layout page
function refreshLedLayout() {
  byId('ledCount').value=info.leds.count
}

function onLayoutChange() {
  var type = byId('ledLayoutType').value
  queryAll('#pLedLayout .page-layout').forEach(e => {
    e.classList.remove("d-none", "d-block")
    e.classList.add(e.dataset.page == type ? "d-block" : "d-none")
  })
}

function onLayoutCodeChange() {
  var leds
  var code = byId('code').value
  try {
    var pts = eval(`var x=${code};if(typeof x==='function'){x()}else{x}`)
    if (!Array.isArray(pts)) throw ""
    leds = pts.length
    console.log(pts)
  } catch (ex) {
    console.log("Error: " + ex)
  }
  setCodeLeds(leds)
}

function setCodeLeds(count) {
  byId("codeLeds").innerHTML = count === undefined ? '&#9888;' : count
}

async function onLayoutSave() {
  // TODO: Check if valid
  var settings = null
  switch (byId('ledLayoutType').value) {
    case 'strip':
      let ledCount = byId('ledCount').value
      let coords = ''
      for (let i = 0; i < ledCount; i++) {
        if (i > 0) coords += ','
        coords += Math.round(10000*(i+.5)/ledCount)/10000 + ',0,0'
      }
      settings = {
        ledCount: ledCount,
        ledLayout: {
          config: '{"layout":"strip"}',
          coords: coords
        }
      }
      break
  }
  if (settings) {
    info.leds.count = settings.ledCount
    info.leds.layout = settings.ledLayout
    await fetch('/api/leds/settings',{
      method:'PATCH',
      body:JSON.stringify(settings)
    })
  }
  showPage('pSettings')
}

// About page
function showAbout() {
  showPage('pAbout')
  updateFps()
}

async function updateFps() {
  let response = await fetch('/api/leds/play/fps')
  if (response.ok) {
    let fps = await response.text()
    byId('fps').innerText = `${fps} fps`
    refreshTimer = setTimeout(updateFps,2000)
  } else {
    refreshTimer = 0
  }
}