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
let ledLayout
let refreshTimer

// Utility functions
function query(s) { 
  return document.querySelector(s)
}
function queryAll(s) { 
  return document.querySelectorAll(s) 
}
function byId(i) {
  return document.getElementById(i) 
}
function posIntKey(e) { 
  if(e.key === "." || e.key === "-") e.preventDefault() 
}

// App load
async function onload() {
  try {
    let response = await fetch('/api/info')
    if (!response.ok) throw `Error: ${response.status}`
    info = await response.json()
  } catch (e) {
    console.log(e)
  }
  refreshPlay()
  refreshLibrary()
}

// App
function refreshPlay() {
  if (info.leds.animations.length) {
    byId('play-name').innerText = info.leds.animations[info.leds.play.index].name
  }
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
  fetch('/api/leds/play?index=' + i, {method:'POST'})
  info.leds.play.index = i
  refreshPlay()
}

// Led Layout page
function copyLedLayout() {
  ledLayout = structuredClone(info.leds.layout)
  try {
    ledLayout.config = JSON.parse(ledLayout.config)
  } catch {
    ledLayout.config = {}
  }
}

function showLedLayout() {
  copyLedLayout()
  showPage('pLedLayout')
  refreshLedLayout()
  refreshCodeInfo()
  refreshLayoutSave()
}

function refreshLedLayout() {
  byId('ledCount').value = info.leds.count
  let type = ledLayout.config.type||'strip'
  byId('ledLayoutType').value = type
  queryAll('#pLedLayout .page-layout').forEach(e => {
    e.style.display = e.dataset.page == type ? 'block' : 'none'
  })
  byId('code').value = ledLayout.config.code||''
}

function refreshCodeInfo() {
  let coords
  if (ledLayout.config.code?.trim().length) {
    try {
      let pts = eval(`var x=${ledLayout.config.code};if(typeof x==='function'){x()}else{x}`)
      if (!Array.isArray(pts)) {
        throw ''
      }
      coords = []
      pts.forEach(pt => {
        if (Array.isArray(pt)) {
          for(let i = 0; i < 3; i++) {
            if (i < pt.length && typeof pt[i] === 'number') {
              coords.push(Math.round(10000*pt[i])/10000)
            } else {
              coords.push(0)
            }
          }
        }
      })
    } catch (ex) {
      coords = undefined
      console.log('Error: ' + ex)
    }
  }
  ledLayout.coords = coords ? coords.join(',') : ''
  byId('codeLeds').innerHTML = coords === undefined ? '&#9888;' : coords.length/3
}

function refreshLayoutSave() {
  let canSave = false
  switch (ledLayout.config.type) {
    case 'code':
      canSave = ledLayout.coords?.length
      break
    default: // 'strip'
      canSave = parseInt(byId('ledCount').value) > 0
      break
    }
  byId('saveLayout').disabled = !canSave
}

function onLayoutChange() {
  ledLayout.config.type = byId('ledLayoutType').value
  refreshLedLayout()
  refreshLayoutSave()
}

function onLayoutCodeChange() {
  ledLayout.config.code = byId('code').value
  refreshCodeInfo()
  refreshLayoutSave()
}

async function onLayoutSave() {
  let count
  let layout
  switch (ledLayout.config.type) {
    case 'code':
      count = ledLayout.coords.split(',').length/3
      layout = {
        config: JSON.stringify(ledLayout.config),
        coords: ledLayout.coords
      }
      break
    default: // 'strip'
      count = parseInt(byId('ledCount').value)
      let coords = []
      for (let i = 0; i < count; i++) {
        let coord = Math.round(10000*(i+.5)/count)/10000
        console.log(coord)
        coords.push(coord + ',' + coord + ',' + coord)
      }
      layout = {
        config: '{"layout":"strip"}',
        coords: coords.join(',')
      }
      break
}
  if (layout) {
    info.leds.count = count
    info.leds.layout = layout
    await fetch('/api/leds/settings',{
      method:'PATCH',
      body:JSON.stringify({
        ledCount: count,
        ledLayout: layout
      })
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