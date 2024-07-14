/**
 * Global fields are available on the options page for every animation
 */
let globalFields = [
  {
    id: 'brightness',
    type: 'slider',
    label: 'Brightness',
    min: 0,
    max: 100,
    factor: 2.55,
    template: '`${val}%`',
    decimals: 0
  }
]
/**
 * Model object with all the information returned from the MCU about the system
 */
let info
/**
 * A javscript timer id for the play preview request
 */
let playPreviewTimer
/**
 * Model object for data while it's being edited
 */
let editing
/**
 * A javascript timer id for periodically freshing a UI element from the server (such as fps)
 */
let refreshTimer

/**
 * Handler for <input type="number" /> that can only be a positive integer value
 * @param {KeyboardEvent} event
 */ 
function posIntKey(event) { 
  if(event.key === "." || event.key === "-") e.preventDefault() 
}

function textColor(background) {
  if ((background.r*0.299 + background.g*0.587 + background.b*0.114) > 160) {
    return {r: 0, g: 0, b: 0}
  } else {
    return {r: 255, g: 255, b: 255}
  }
}

function toHex(rgb) {
  let hex = '#';
  [rgb.r, rgb.g, rgb.b].forEach(v => {
    hex += v.toString(16).padStart(2,'0')
  })
  return hex
}

function rgb2hsv(rgb) {
  let r = rgb.r/255, g = rgb.g/255, b = rgb.b/255
  let v = Math.max(r, g, b), c = v-Math.min(r, g, b)
  let h = c && ((v == r) ? (g-b)/c : ((v == g) ? 2 + (b-r)/c : 4+(r-g)/c))
  return {
    h: Math.round(42.5*(h < 0 ? h+6 : h)),
    s: Math.round(255*(v&&c/v)),
    v: Math.round(255*v)
  }
}

function hsv2rgb(hsv) {
  let h = hsv.h*360/255, s = hsv.s/255, v = hsv.v/255
  let f = (n,k=(n+h/60)%6) => v - v*s*Math.max( Math.min(k,4-k,1), 0)
  return {
    r: Math.round(255*f(5)),
    g: Math.round(255*f(3)),
    b: Math.round(255*f(1))
  }
} 

/**
 * App UI initialization. Fetch global data from the server and refresh the UI
 */
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
  refreshColors()

  setInterval(refreshPlayPreview, 1000)
  refreshPlayPreview()
}

/**
 * Refresh the currently playing animation info
 */
function refreshPlay() {
  if (info.leds.animations.length) {
    document.getElementById('play-name').innerText = info.leds.animations[info.leds.play.index].name
  }
}

async function refreshPlayPreview() {
  const response = await fetch('/api/leds/play/rgb')
  if (response.ok) {
    const bmpData = await response.arrayBuffer()
    const blob = new Blob([bmpData], { type: 'image/bmp' })
    const reader = new FileReader()
    reader.onloadend = () => {
      document.getElementById('playPreview').src = reader.result
    }
    reader.readAsDataURL(blob)
  }
}

/**
 * Set a page to visible
 * @param {string} id - Element id
 */
function showPage(id) {
  if (refreshTimer) {
    clearTimeout(refreshTimer)
    refreshTimer = 0
  }
  document.querySelectorAll('.nav > div').forEach(e => {
    e.classList.remove('selected')
    if (e.dataset.page == id) {
      e.classList.add('selected')
    }
  })
  document.querySelectorAll('.main > div').forEach(e => {
    e.classList.remove('d-flex','d-none')
    e.classList.add(e.id == id ? 'd-flex' : 'd-none')
  })
}

// -----------------------------------------------------------------------------
// Play options page
// -----------------------------------------------------------------------------

/**
 * Get the field information for the specified id from either the global fields or the animation's fields
 * @param {string} id - The field id
 */
// Play options page
function optionsField(id) {
  return globalFields.find(f => f.id == id)|| info.leds.animations[info.leds.play.index].fields?.find( f=> f.id == id)
}

/**
 * Create the controls for the options page from the currently running animation and make the page visible
 */
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
    html += optionHtml(optionsField(f))
  }
  list.innerHTML = html
  for (let f of fields) {
    refreshOptionControls(optionsField(f))
  }
  showPage('pPlayOptions')
}

/**
 * Get the HTML for the specified field using the control templates in HTML scripts
 * @param field - Field information
 */
function optionHtml(field) {
  let id = field.id
  let label = field.label
  let range = [field.min, field.max]
  let rangeLabels = []
  range.forEach(val => {
    rangeLabels.push(field.template ? eval(field.template) : val)
  })

  switch (field.type) {
    case 'slider':
      return eval('`' + document.getElementById('tSlider').innerHTML + '`')
    case 'hue':
      return eval('`' + document.getElementById('tColor').innerHTML + '`')
    default:
      return ''
  }
}

/**
 * Refresh the controls for the specified field using the play or animation model settings
 * @param field - Field information
 */
function refreshOptionControls(field) {
  let val = info.leds.play.settings[field.id] || info.leds.animations[info.leds.play.index].settings[field.id]
  if (field.factor) val /= field.factor

  let itemElement = document.querySelector(`.page.options [data-field="${field.id}"]`)
  switch (field.type) {
    case 'slider':
    case 'hue':
      let input = itemElement.querySelector('input')
      input.value = val
      break
  }
  refreshOptionValue(itemElement, val, field)
}

/**
 * Refresh the controls for a specific item with the value
 * @param {HTMLElement} element - Top level item option div element
 * @param val - The value
 * @param field - Field information
 */
function refreshOptionValue(element, val, field) {
  if (field.decimals != undefined) {
    let multiple = Math.pow(10, field.decimals)
    val = Math.round(val*multiple)/multiple
  }
  switch (field.type) {
    case 'slider':
      if (field.template) val = eval(field.template)
        element.querySelector('.value').innerHTML = val
      break
    case 'hue':
      val *= (360/255)
      element.querySelector('.value').style.backgroundColor=`hsl(${val},100%,50%)`
      break
  }
}

/**
 * Handle the event when an option value changes
 * @param {HTMLElement} element - The element that is changing
 */
async function onOptionChange(element) {
  let itemElement = element.closest('.item')
  let id = itemElement.dataset.field
  let field = optionsField(id)
  let isGlobal = globalFields.some(f => f.id == id)
  let val = element.value
  let modelVal = val
  if (field.factor) {
    modelVal *= field.factor
  }
  if (isGlobal) {
    info.leds.play.settings[id] = modelVal
  } else {
    info.leds.animations[info.leds.play.index].settings[id] = modelVal
  }
  refreshOptionValue(itemElement, val, field)
  let patch = {}
  patch[id] = modelVal
  if (isGlobal) {
    await fetch('/api/leds/play/settings', {
      method:'PATCH',
      body:JSON.stringify(patch)
    })
  } else {
    patch.index = info.leds.play.index
    await fetch('/api/leds/animations/settings', {
      method:'PATCH',
      body:JSON.stringify(patch)
    })
  }
}

// -----------------------------------------------------------------------------
// Library page
// -----------------------------------------------------------------------------

/**
 * Refresh the library page from the animations collection
 */
function refreshLibrary() {
  let html = ''
  for(let i = 0; i < info.leds.animations.length; i++) {
    let selected = i == info.leds.play.index ? ' selected' : ''
    html += '<div class="item pa-3' + selected + '" onclick="onAnimationClick(' + i + ')"><div class="text">' + info.leds.animations[i].name + '</div></div>'
  }
  document.getElementById('animations').innerHTML = html
}

/**
 * Handle the click event on the animation list to make it actively playing
 * @param {number} index - The animation index clicked
 */
function onAnimationClick(index) {
  fetch('/api/leds/play?index=' + index, {method:'POST'})
  info.leds.play.index = index
  refreshPlay()
  refreshLibrary()
}

// -----------------------------------------------------------------------------
// Led Layout page
// -----------------------------------------------------------------------------

/**
 * Clone the LED layout information from the global settings so it can be modified by controls prior to saving
 */
function copyLedLayout() {
  editing = structuredClone(info.leds.layout)
  try {
    editing.config = JSON.parse(editing.config)
  } catch {
    editing.config = {}
  }
}

/**
 * Refresh the controls from the model and make the page visible
 */
function showLedLayout() {
  copyLedLayout()
  showPage('pLedLayout')
  refreshLedLayout()
  refreshCodeInfo()
  refreshLayoutSave()
}

/**
 * Refresh the controls from the model
 */
function refreshLedLayout() {
  document.getElementById('ledCount').value = info.leds.count
  let type = editing.config.type||'strip'
  document.getElementById('ledLayoutType').value = type
  document.querySelectorAll('#pLedLayout .page-layout').forEach(e => {
    e.style.display = e.dataset.page == type ? 'block' : 'none'
  })
  document.getElementById('code').value = editing.config.code||''
}

/**
 * Attempt to parse the text in the code input into an array of 3d coordinates.
 * If successful update the model with the coordinate info and led count
 */
function refreshCodeInfo() {
  let coords
  if (editing.config.code?.trim().length) {
    try {
      let pts = eval(`var x=${editing.config.code};if(typeof x==='function'){x()}else{x}`)
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
  editing.coords = coords ? coords.join(',') : ''
  document.getElementById('codeLeds').innerHTML = coords === undefined ? '&#9888;' : coords.length/3
}

/**
 * Refresh the save button enabled state based on whether a valid LED layout has been specified.
 * For now all that is required is the total number of leds is > 0
 */
function refreshLayoutSave() {
  let canSave = false
  switch (editing.config.type) {
    case 'code':
      canSave = editing.coords?.length
      break
    default: // 'strip'
      canSave = parseInt(document.getElementById('ledCount').value) > 0
      break
    }
  document.getElementById('saveLayout').disabled = !canSave
}

/**
 * Handle a layout type change by showing the relevant options page for that LED layout
 */
function onLayoutTypeChange() {
  editing.config.type = document.getElementById('ledLayoutType').value
  refreshLedLayout()
  refreshLayoutSave()
}

/**
 * Handle a change to the text in the code input
 */
function onLayoutCodeChange() {
  editing.config.code = document.getElementById('code').value
  refreshCodeInfo()
  refreshLayoutSave()
}

/**
 * Handle the save button click by saving the LED Layout to the server
 */
async function onLayoutSave() {
  let count
  let layout
  switch (editing.config.type) {
    case 'code':
      count = editing.coords.split(',').length/3
      layout = {
        config: JSON.stringify(editing.config),
        coords: editing.coords
      }
      break
    default: // 'strip'
      count = parseInt(document.getElementById('ledCount').value)
      let coords = []
      for (let i = 0; i < count; i++) {
        let coord = Math.round(10000*(i+.5)/count)/10000
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

// -----------------------------------------------------------------------------
// Settings > Colors page
// -----------------------------------------------------------------------------

/**
 * Refresh the page with colors
 */
function refreshColors() {
  let html = ''
  let index = 0
  info.leds.colors.forEach(bg => {
    let text = textColor(bg)
    html += `<div onclick="onColorClick(${index})" class="color selectable" style="background-color:${toHex(bg)};color:${toHex(text)}">${index+1}</div>`
    index++
  });
  document.querySelector('#pColors .grid').innerHTML = html
}

/**
 * Handle click to edit a color
 * @param index 
 */
function onColorClick(index) {
  editing = index
  document.querySelector('#pColorEdit .title .text').innerText = `Edit Color ${index+1}`
  refreshColorEdit()
  showPage('pColorEdit')
}

/**
 * Refresh the details of the edit color page
 */
function refreshColorEdit(field) {
  let rgb = info.leds.colors[editing]
  var colorElement = document.querySelector('#pColorEdit .color')
  colorElement.style.backgroundColor = toHex(rgb)
  colorElement.style.color = toHex(textColor(rgb))
  let hsv = rgb2hsv(rgb)
  document.querySelectorAll('#pColorEdit [data-type="hsv"]').forEach(element => {
    if (element.dataset.field != field) {
      element.value = hsv[element.dataset.field]
    }
  })
}

/**
 * Handle a control change
 * @param {HTMLElement} element - The element whose value is changing
 */
async function onColorChange(element) {
  let hsv = {}
  document.querySelectorAll('#pColorEdit [data-type="hsv"]').forEach(element => {
    hsv[element.dataset.field] = element.value
  })
  let rgb = info.leds.colors[editing]
  for (const [key, value] of Object.entries(hsv2rgb(hsv))) {
    rgb[key] = value
  }
  refreshColorEdit(element.dataset.field)
  refreshColors()
  await fetch('/api/leds/colors', {
    method:'PUT',
    body:JSON.stringify({
      index: editing,
      rgb: rgb
    })
  })
}


// -----------------------------------------------------------------------------
// About page
// -----------------------------------------------------------------------------

/**
 * Show the about page and start a timer to update the frames per second periodically from the server
 */
function showAbout() {
  showPage('pAbout')
  updateFps()
}

/**
 * Query the server for the current frames per second, update the UI, and set a timer to check again in 2 seconds
 */
async function updateFps() {
  let response = await fetch('/api/leds/play/fps')
  if (response.ok) {
    let fps = await response.text()
    document.getElementById('fps').innerText = `${fps} fps`
    refreshTimer = setTimeout(updateFps, 2000)
  } else {
    refreshTimer = 0
  }
}