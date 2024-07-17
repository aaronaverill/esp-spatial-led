class App {
  /**
   * App UI initialization. Fetch global data from the server and refresh the UI
   */
  async init() {
    try {
      let response = await fetch('/api/info')
      if (!response.ok) throw `Error: ${response.status}`
      this.#info = await response.json()
    } catch (e) {
      console.log(e)
    }
    this.#refreshPlay()
    this.#refreshLibrary()
    this.#refreshColors()
  
    this.#playRefreshInterval = setInterval(this.#refreshPlayPreview, 1000)
    this.#refreshPlayPreview()
  }

  /**
   * Set a page to visible
   * @param {string} id - Element id
   */
  showPage(id) {
    if (this.#refreshInterval) {
      clearInterval(this.#refreshInterval)
      this.#refreshInterval = 0
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

  async #refreshPlayPreview() {
    let response = { ok: false }
    try {
      response = await fetch('/api/leds/play/rgb')
    } catch { }
    if (response.ok) {
      let bmpData = await response.arrayBuffer()
      let blob = new Blob([bmpData], { type: 'image/bmp' })
      let reader = new FileReader()
      reader.onloadend = () => {
        document.getElementById('playPreview').src = reader.result
      }
      reader.readAsDataURL(blob)
    } else {
      clearInterval(this.#playRefreshInterval)
      this.#playRefreshInterval = 0
    }
  }

  /**
   * Refresh the currently playing animation info
   */
  #refreshPlay() {
    if (this.#info.leds.animations.length) {
      document.getElementById('play-name').innerText = this.#info.leds.animations[this.#info.leds.play.index].name
    }
  }

  // -----------------------------------------------------------------------------
  // Play options page
  // -----------------------------------------------------------------------------

  /**
   * Create the controls for the options page from the currently running animation and make the page visible
   */
  showOptions() {
    let a = this.#info.leds.animations[this.#info.leds.play.index]
    let list = document.querySelector('.page.options .list')
    let html = ''
    let fields = []
    for (let f of this.#globalFields) fields.push(f.id)
    if (a.fields) {
      for(let f of a.fields) fields.push(f.id)
    }
    for (let f of fields) {
      html += this.#optionHtml(this.#optionsField(f))
    }
    list.innerHTML = html
    for (let f of fields) {
      this.#refreshOptionControls(this.#optionsField(f))
    }
    this.showPage('pPlayOptions')
  }
  
  /**
   * Handle the event when an option value changes
   * @param {HTMLElement} element - The element that is changing
   */
  async onOptionChange(element) {
    let itemElement = element.closest('.item')
    let id = itemElement.dataset.field
    let field = this.#optionsField(id)
    let isGlobal = this.#globalFields.some(f => f.id == id)
    let val = element.value
    let modelVal = val
    if (field.factor) {
      modelVal *= field.factor
    }
    if (isGlobal) {
      this.#info.leds.play.settings[id] = modelVal
    } else {
      this.#info.leds.animations[this.#info.leds.play.index].settings[id] = modelVal
    }
    this.#refreshOptionValue(itemElement, val, field)
    let patch = {}
    patch[id] = modelVal
    if (isGlobal) {
      await fetch('/api/leds/play/settings', {
        method:'PATCH',
        body:JSON.stringify(patch)
      })
    } else {
      patch.index = this.#info.leds.play.index
      await fetch('/api/leds/animations/settings', {
        method:'PATCH',
        body:JSON.stringify(patch)
      })
    }
  }

  /**
   * Get the field information for the specified id from either the global fields or the animation's fields
   * @param {string} id - The field id
   */
  // Play options page
  #optionsField(id) {
    return this.#globalFields.find(f => f.id == id)|| this.#info.leds.animations[this.#info.leds.play.index].fields?.find( f=> f.id == id)
  }

  /**
   * Get the HTML for the specified field using the control templates in HTML scripts
   * @param field - Field information
   */
  #optionHtml(field) {
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
  #refreshOptionControls(field) {
    let val = this.#info.leds.play.settings[field.id] || this.#info.leds.animations[this.#info.leds.play.index].settings[field.id]
    if (field.factor) val /= field.factor

    let itemElement = document.querySelector(`.page.options [data-field="${field.id}"]`)
    switch (field.type) {
      case 'slider':
      case 'hue':
        let input = itemElement.querySelector('input')
        input.value = val
        break
    }
    this.#refreshOptionValue(itemElement, val, field)
  }

  /**
   * Refresh the controls for a specific item with the value
   * @param {HTMLElement} element - Top level item option div element
   * @param val - The value
   * @param field - Field information
   */
  #refreshOptionValue(element, val, field) {
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

  // -----------------------------------------------------------------------------
  // Library page
  // -----------------------------------------------------------------------------

  /**
   * Handle the click event on the animation list to make it actively playing
   * @param {number} index - The animation index clicked
   */
  onAnimationClick(index) {
    fetch('/api/leds/play?index=' + index, {method:'POST'})
    this.#info.leds.play.index = index
    this.#refreshPlay()
    this.#refreshLibrary()
  }

  /**
   * Refresh the library page from the animations collection
   */
  #refreshLibrary() {
    let html = ''
    for(let i = 0; i < this.#info.leds.animations.length; i++) {
      let selected = i == this.#info.leds.play.index ? ' selected' : ''
      html += '<div class="item pa-3' + selected + '" onclick="app.onAnimationClick(' + i + ')"><div class="text">' + this.#info.leds.animations[i].name + '</div></div>'
    }
    document.getElementById('animations').innerHTML = html
  }

  // -----------------------------------------------------------------------------
  // Settings > Led Layout page
  // -----------------------------------------------------------------------------

  /**
   * Refresh the controls from the model and make the page visible
   */
  showLedLayout() {
    this.#copyLedLayout()
    this.showPage('pLedLayout')
    this.#refreshLedLayout()
    this.#refreshCodeInfo()
    this.#refreshLayoutSave()
  }

  /**
   * Handle the save button click by saving the LED Layout to the server
   */
  async onLayoutSave() {
    await this.#saveLedLayout()
    this.showPage('pSettings')
  }

  /**
   * Handle a layout type change by showing the relevant options page for that LED layout
   */
  onLayoutTypeChange() {
    this.#editing.config.type = document.getElementById('ledLayoutType').value
    this.#refreshLedLayout()
    this.#refreshLayoutSave()
  }

  /**
   * Refresh the save button enabled state when the layout count changes
   */
  onLayoutLedCountChange() {
    this.#refreshLayoutSave()
  }

  /**
   * Handle a change to the text in the code input
   */
  onLayoutCodeChange() {
    this.#editing.config.code = document.getElementById('code').value
    this.#refreshCodeInfo()
    this.#refreshLayoutSave()
  }
    
  /**
   * Clone the LED layout information from the global settings so it can be modified by controls prior to saving
   */
  #copyLedLayout() {
    this.#editing = structuredClone(this.#info.leds.layout)
    try {
      this.#editing.config = JSON.parse(this.#editing.config)
    } catch {
      this.#editing.config = {}
    }
  }

  /**
   * Refresh the controls from the model
   */
  #refreshLedLayout() {
    document.getElementById('ledCount').value = this.#info.leds.count
    let type = this.#editing.config.type||'strip'
    document.getElementById('ledLayoutType').value = type
    document.querySelectorAll('#pLedLayout .page-layout').forEach(e => {
      e.style.display = e.dataset.page == type ? 'block' : 'none'
    })
    document.getElementById('code').value = this.#editing.config.code||''
  }

  /**
   * Attempt to parse the text in the code input into an array of 3d coordinates.
   * If successful update the model with the coordinate info and led count
   */
  #refreshCodeInfo() {
    let coords
    if (this.#editing.config.code?.trim().length) {
      try {
        let code = this.#editing.config.code
        let pts = eval(`var x=${code};if(typeof x==='function'){x()}else{x}`)
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
    this.#editing.coords = coords ? coords.join(',') : ''
    document.getElementById('codeLeds').innerHTML = coords === undefined ? '&#9888;' : coords.length/3
  }

  /**
   * Refresh the save button enabled state based on whether a valid LED layout has been specified.
   * For now all that is required is the total number of leds is > 0
   */
  #refreshLayoutSave() {
    let canSave = false
    switch (this.#editing.config.type) {
      case 'code':
        canSave = this.#editing.coords?.length
        break
      default: // 'strip'
        canSave = parseInt(document.getElementById('ledCount').value) > 0
        break
      }
    document.getElementById('saveLayout').disabled = !canSave
  }

  /**
   * Save the LED Layout to the server
   */
  async #saveLedLayout() {
    let count
    let layout
    switch (this.#editing.config.type) {
      case 'code':
        count = this.#editing.coords.split(',').length/3
        layout = {
          config: JSON.stringify(this.#editing.config),
          coords: this.#editing.coords
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
      this.#info.leds.count = count
      this.#info.leds.layout = layout
      await fetch('/api/leds/settings',{
        method:'PATCH',
        body:JSON.stringify({
          ledCount: count,
          ledLayout: layout
        })
      })
    }
  }

  // -----------------------------------------------------------------------------
  // Settings > Colors page
  // -----------------------------------------------------------------------------

  /**
   * Handle click to edit a color
   * @param index 
   */
  onColorClick(index) {
    this.#editing = index
    document.querySelector('#pColorEdit .title .text').innerText = `Edit Color ${index+1}`
    this.#refreshColorEdit()
    this.showPage('pColorEdit')
  }

  /**
   * Handle a control change
   * @param {HTMLElement} element - The element whose value is changing
   */
  async onColorChange(element) {
    let hsv = {}
    document.querySelectorAll('#pColorEdit [data-type="hsv"]').forEach(element => {
      hsv[element.dataset.field] = element.value
    })
    let rgb = this.#info.leds.colors[this.#editing]
    for (const [key, value] of Object.entries(this.#hsv2rgb(hsv))) {
      rgb[key] = value
    }
    this.#refreshColorEdit(element.dataset.field)
    this.#refreshColors()
    await fetch('/api/leds/colors', {
      method:'PUT',
      body:JSON.stringify({
        index: this.#editing,
        rgb: rgb
      })
    })
  }

  /**
   * Handle a click in the rainbow strip below the hue slider to pick a hue
   * @param {HTMLElement} element 
   * @param {PointerEvent} event 
   */
  async onColorHueClick(element, event) {
    var hue = Math.floor(event.offsetX/element.clientWidth*255)
    console.log(hue)
    var inputElement = element.closest('.item').querySelector('input')
    inputElement.value = hue
    this.onColorChange(inputElement)
  }
  
  /**
   * Refresh the details of the edit color page
   */
  #refreshColorEdit(field) {
    let rgb = this.#info.leds.colors[this.#editing]
    var colorElement = document.querySelector('#pColorEdit .color')
    colorElement.style.backgroundColor = this.#toHex(rgb)
    colorElement.style.color = this.#toHex(this.#textColor(rgb))
    let hsv = this.#rgb2hsv(rgb)
    document.querySelectorAll('#pColorEdit [data-type="hsv"]').forEach(element => {
      if (element.dataset.field != field) {
        element.value = hsv[element.dataset.field]
      }
    })
  }

  /**
   * Refresh the page with colors
   */
  #refreshColors() {
    let html = ''
    let index = 0
    this.#info.leds.colors.forEach(bg => {
      let text = this.#textColor(bg)
      let textHex = this.#toHex(text)
      let bgHex = this.#toHex(bg)
      html += `<div onclick="app.onColorClick(${index})" class="color selectabl rounded" style="color:${textHex};background-color:${bgHex}">${index+1}</div>`
      index++
    });
    document.querySelector('#pColors .grid').innerHTML = html
  }

  // -----------------------------------------------------------------------------
  // About page
  // -----------------------------------------------------------------------------

  /**
   * Show the about page and start a timer to update the frames per second periodically from the server
   */
  showAbout() {
    app.showPage('pAbout')
    this.#refreshInterval = setInterval(this.#updateFps, 2000)
    this.#updateFps()
  }

  /**
   * Query the server for the current frames per second, update the UI, and set a timer to check again in 2 seconds
   */
  async #updateFps() {
    let response = { ok: false }
    try {
      response = await fetch('/api/leds/play/fps')
    } catch {}
    if (response.ok) {
      let fps = await response.text()
      document.getElementById('fps').innerText = `${fps} fps`
    } else {
      document.getElementById('fps').innerText = 'fps unavailable'
      clearInterval(this.#refreshInterval)
      this.#refreshInterval = 0
    }
  }

  // -----------------------------------------------------------------------------
  // Utility functions
  // -----------------------------------------------------------------------------

  /**
   * Handler for <input type="number" /> that can only be a positive integer value
   * @param {KeyboardEvent} event
   */ 
  onKeyPositiveInt(event) { 
    if(['.', '-', '+', 'e'].includes(event.key)) {
      event.preventDefault() 
    }
  }

  #textColor(background) {
    if ((background.r*0.299 + background.g*0.587 + background.b*0.114) > 160) {
      return {r: 0, g: 0, b: 0}
    } else {
      return {r: 255, g: 255, b: 255}
    }
  }
  
  #toHex(rgb) {
    let hex = '#';
    [rgb.r, rgb.g, rgb.b].forEach(v => {
      hex += v.toString(16).padStart(2,'0')
    })
    return hex
  }
  
  #rgb2hsv(rgb) {
    let r = rgb.r/255, g = rgb.g/255, b = rgb.b/255
    let v = Math.max(r, g, b), c = v-Math.min(r, g, b)
    let h = c && ((v == r) ? (g-b)/c : ((v == g) ? 2 + (b-r)/c : 4+(r-g)/c))
    return {
      h: Math.round(42.5*(h < 0 ? h+6 : h)),
      s: Math.round(255*(v&&c/v)),
      v: Math.round(255*v)
    }
  }
  
  #hsv2rgb(hsv) {
    let h = hsv.h*360/255, s = hsv.s/255, v = hsv.v/255
    let f = (n,k=(n+h/60)%6) => v - v*s*Math.max( Math.min(k,4-k,1), 0)
    return {
      r: Math.round(255*f(5)),
      g: Math.round(255*f(3)),
      b: Math.round(255*f(1))
    }
  }

  /**
   * Public method to set info for testing locally
   */
  setInfo(info) {
    this.#info = info
  }

  /**
   * Model object with all the information returned from the MCU about the system
   */
  #info
  /**
   * Model object for data while it's being edited
   */
  #editing
  /**
   * A javascript interval id for periodically freshing the play preview image
   */
  #playRefreshInterval
  /**
   * A javascript interval id for periodically freshing a UI element from the server (such as fps)
   */
  #refreshInterval
  /**
   * Global fields are available on the options page for every animation
   */
  #globalFields = [
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
}

let app = new App()