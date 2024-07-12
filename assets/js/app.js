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
    template: '${val}%'
  }
]
/**
 * Model object with all the information returned from the MCU about the system
 */
let info
/**
 * Model object for the LED Layout page while it's being edited
 */
let ledLayout
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
}

/**
 * Refresh the currently playing animation info
 */
function refreshPlay() {
  if (info.leds.animations.length) {
    document.getElementById('play-name').innerText = info.leds.animations[info.leds.play.index].name
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
    rangeLabels.push(field.template ? eval(`\`${field.template}\``) : val)
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
  refreshOptionValue(itemElement, Math.round(val), field)
}

/**
 * Refresh the controls for a specific item with the value
 * @param {HTMLElement} element - Top level item option div element
 * @param val - The value
 * @param field - Field information
 */
function refreshOptionValue(element, val, field) {
  switch (field.type) {
    case 'slider':
      if (field.template) val = eval(`\`${field.template}\``)
        element.querySelector('.value').innerText = val
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
  modelVal = Math.round(modelVal)
  if (isGlobal) {
    info.leds.play.settings[id] = modelVal
  } else {
    info.leds.animations[info.leds.play.index].settings[id] = modelVal
  }
  refreshOptionValue(itemElement, val, field)
  let patch = {}
  patch[id] = modelVal
  if (isGlobal) {
    await fetch('/api/leds/play/settings',{
      method:'PATCH',
      body:JSON.stringify(patch)
    })
  } else {
    fetch(`/api/leds/animations/settings?index=${info.leds.play.index}&${id}=${modelVal}`,{method:'POST'})
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
  ledLayout = structuredClone(info.leds.layout)
  try {
    ledLayout.config = JSON.parse(ledLayout.config)
  } catch {
    ledLayout.config = {}
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
  let type = ledLayout.config.type||'strip'
  document.getElementById('ledLayoutType').value = type
  document.querySelectorAll('#pLedLayout .page-layout').forEach(e => {
    e.style.display = e.dataset.page == type ? 'block' : 'none'
  })
  document.getElementById('code').value = ledLayout.config.code||''
}

/**
 * Attempt to parse the text in the code input into an array of 3d coordinates.
 * If successful update the model with the coordinate info and led count
 */
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
  document.getElementById('codeLeds').innerHTML = coords === undefined ? '&#9888;' : coords.length/3
}

/**
 * Refresh the save button enabled state based on whether a valid LED layout has been specified.
 * For now all that is required is the total number of leds is > 0
 */
function refreshLayoutSave() {
  let canSave = false
  switch (ledLayout.config.type) {
    case 'code':
      canSave = ledLayout.coords?.length
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
  ledLayout.config.type = document.getElementById('ledLayoutType').value
  refreshLedLayout()
  refreshLayoutSave()
}

/**
 * Handle a change to the text in the code input
 */
function onLayoutCodeChange() {
  ledLayout.config.code = document.getElementById('code').value
  refreshCodeInfo()
  refreshLayoutSave()
}

/**
 * Handle the save button click by saving the LED Layout to the server
 */
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
      count = parseInt(document.getElementById('ledCount').value)
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
    refreshTimer = setTimeout(updateFps,2000)
  } else {
    refreshTimer = 0
  }
}