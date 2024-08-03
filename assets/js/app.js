import throttle from './throttle.js'
import Led3D from './Led3D.js'

export default class App {
  constructor() {
    // Update play setting options once per second
    this.#throttledSavePlaySettings = throttle((patch) => {
      this.#savePlaySettings(patch)
    }, 1000)

    // Update animation setting options once per second
    this.#throttledSaveAnimationSettings = throttle((patch) => {
      this.#saveAnimationSettings(patch)
    }, 1000)

    // Update color info once per second
    this.#throttledSaveColor = throttle((patch) => {
      this.#saveColor(patch)
    }, 1000)

    // Update palette info once per second
    this.#throttledSavePalette = throttle((patch) => {
      this.#savePalette(patch)
    }, 1000)
  }

  /**
   * App UI initialization. Fetch global data from the server and refresh the UI
   */
  async init() {
    try {
      const response = await fetch('/api/info')
      if (!response.ok) throw `Error: ${response.status}`
      this.#info = await response.json()
    } catch (e) {
      console.log(e)
    }
    this.#refreshPlay()
    this.#refreshLibrary()
    this.#refreshLedStrip()
  
    this.#playRefreshInterval = setInterval(() => {this.#refreshPlayPreview()}, 500)
    this.#refreshPlayPreview()
  }

  /**
   * Set a page to visible
   * @param {string} id - Element id
   */
  set page(id) {
    if (this.#refreshInterval) {
      clearInterval(this.#refreshInterval)
      this.#refreshInterval = 0
    }
    switch (id) {
      case 'pPlayOptions':
        this.#refreshAnimationOptions()
        break
      case 'pLedLayout':
        this.#copyLedLayout()
        this.#refreshLedLayout()
        this.#refreshCodeInfo()
        this.#refreshLayoutSave()
        break
      case 'pColors':
        this.#refreshColors()
        break
      case 'pPalettes':
        this.#refreshPalettes()
        break
      case 'pAbout':
        this.#refreshInterval = setInterval(this.#updateSystemInfo, 2000)
        this.#updateSystemInfo()
        break
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

  /**
   * Query the server for the currently playing R,G,B values and update the preview bitmap and visualization color arrays
   */
  async #refreshPlayPreview() {
    let response = { ok: false }
    try {
      response = await fetch('/api/leds/play/buffer')
    } catch { }
    if (response.ok) {
      const ledBuffer = await response.arrayBuffer()
      const bufferView = new Uint8Array(ledBuffer)
      if (bufferView.length < 4) {
        return
      }

      const pixelSize = bufferView[0]
      const indexes = [1, 2, 3].map(index => bufferView[index])
      const ledCount = (bufferView.length - pixelSize - 1) / pixelSize
      const rgbBrightness = 255/this.#info.leds.play.settings.brightness

      // Copy the buffer data into an rgb array for the layout preview
      const rgbData = new ArrayBuffer(ledCount * 3)
      const rgbView = new Uint8Array(rgbData)
      let p = 0
      for(let i = 0; i < rgbView.length; i++) {
        rgbView[p + indexes[i%3]] = bufferView[1 + pixelSize + i] * rgbBrightness
        if (i%3 == 2) {
          p += 3
        }
      }
      document.getElementById('layoutPreview').colorArray = rgbData

      // Copy the buffer data into a bitmap - green, red, blue with a header
      const rgbSize = ledCount * 3
      const rowSize = Math.ceil(rgbSize / 4) * 4
      const bmpHeaderSize = 54
      const bmpSize = bmpHeaderSize + rowSize

      const bmpData = new ArrayBuffer(bmpSize)
      const bmpView = new Uint8Array(bmpData)

      // Construct the header
      bmpView.set([0x42, 0x4D], 0) // "BM"
      bmpView.set([bmpSize & 0xFF, (bmpSize >> 8) & 0xFF], 2) // Size of bitmap file, little endian
      bmpView[10] = bmpHeaderSize // Start of bitmap data, after headers
      bmpView[14] = 40 // Size of hedaer
      bmpView.set([ledCount & 0xFF, (ledCount >> 8) & 0xFF], 18) // Width in pixels
      bmpView[22] = 1 // Height in pixel
      bmpView[26] = 1 // Number of color planes
      bmpView[28] = 24 // Bits per pixel
      bmpView.set([rowSize & 0xFF, (rowSize >> 8) & 0xFF], 34) // Row size
      
      // Reorder the remaining bytes from RGB to BGR
      for (let i = 0; i < rgbView.length; i += 3) {
        bmpView[bmpHeaderSize + i] = rgbView[i + 2] // Blue
        bmpView[bmpHeaderSize + i + 1] = rgbView[i + 1] // Green
        bmpView[bmpHeaderSize + i + 2] = rgbView[i] // Red
      }

      // Read the data into a URL we can use to set the src attribute of the preview img element
      const blob = new Blob([bmpData], { type: 'image/bmp' })
      const reader = new FileReader()
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
   * Handle the event when an option value changes
   * @param {HTMLElement} element - The element that is changing
   */
  async onOptionChange(element) {
    const itemElement = element.closest('.item')
    const id = itemElement.dataset.field
    const field = this.#optionsField(id)
    const val = new Number(element.value)
    let modelVal = val
    if (field.factor) {
      modelVal *= field.factor
    }
    await this.#setOptionModelAndSave(itemElement, id, modelVal, val)
  }

  /**
   * Handle a change to the hue slider for an option color field
   * @param {HTMLElement} element - The field's element being updated
   */
  async onOptionColorHueChange(element) {
    const itemElement = element.closest('.item')
    const id = itemElement.dataset.field
    const hue = element.value
    const modelVal = {
      palette: 0,
      color: 0,
      rgb: this.#hsv2rgb({h: hue, s: 255, v: 255})
    }
    await this.#setOptionModelAndSave(itemElement, id, modelVal, modelVal)
  }

  /**
   * Handle a control change
   * @param {HTMLElement} element - The element whose value is changing
   */
  async onAnimationColorChange(element) {
    const setting = this.#info.leds.animations[this.#info.leds.play.index].settings[this.#editing]
    const rgb = this.#getColorHSVControlRGB('pAnimationColor')
    const color = setting.color

    setting.rgb = [...rgb]

    const isNumberedColor = color && color <= this.#info.leds.colors.length
    if (isNumberedColor) {
      this.#info.leds.colors[color-1] = [...rgb]
    }

    this.#refreshSelectColor()
    this.#refreshAnimationOptions()
  
    if (isNumberedColor) {
      this.#throttledSaveColor({
        index: color-1,
        rgb: rgb
      })
    } else {
      const patch = {
        index: this.#info.leds.play.index,
      }
      patch[this.#editing] = {
        palette: 0,
        color: color,
        rgb: rgb
      }
      this.#throttledSaveAnimationSettings(patch)
    }
  }

  /**
   * Handle a click in the rainbow strip below the hue slider to pick a hue
   * @param {HTMLElement} element 
   * @param {PointerEvent} event 
   */
  async onAnimationHueClick(element, event) {
    var hue = Math.floor(event.offsetX/element.clientWidth*255)
    var inputElement = element.closest('.item').querySelector('input')
    inputElement.value = hue
    this.onAnimationColorChange(inputElement)
  }

  /**
   * Show the palette/color selection page
   * @param {HTMLElement} element - The field's element which was clicked
   */
  onSelectAnimationColor(element) {
    const itemElement = element.closest('.item')
    this.#editing = itemElement.dataset.field
    const field = this.#optionsField(this.#editing)
    let val = this.#info.leds.animations[this.#info.leds.play.index].settings[field.id]
    this.#activeTab = val.palette ? 'palette' : 'color'

    document.querySelector('#pAnimationColor .title .text').innerText = `Select ${field.label}`
    document.querySelector('#pAnimationColor .tabs').style.display = field.palette ? '' : 'none'
    this.#refreshSelectColorActiveTab()
    this.#refreshSelectPaletteList()
    this.#refreshSelectColor()
    this.page = 'pAnimationColor'
  }

  /**
   * Set the active tab visible on the select color page
   * @param {HTMLElement} element - The clicked tab
   */
  setColorActiveTab(element) {
    this.#activeTab = element.dataset.tab
    this.#refreshSelectColorActiveTab()
  }

  /**
   * Handle a click on an animation palette
   * @param {HTMLElement} element - The palette element
   */
  async selectPaletteNumber(element) {
    const number = Array.from(element.parentNode.children).indexOf(element) + 1
    const setting = this.#info.leds.animations[this.#info.leds.play.index].settings[this.#editing]
    setting.palette = number
    setting.color = 0
    this.#refreshSelectPaletteList()
    this.#refreshSelectColor()

    const patch = {
      index: this.#info.leds.play.index
    }
    patch[this.#editing] = setting
    this.#throttledSaveAnimationSettings(patch)
  }

  /**
   * Set the color number to the selected element
   * @param {HTMLElement} element - Color number selected by index
   */
  selectColorNumber(element) {
    const number = Array.from(element.parentNode.children).indexOf(element)
    const setting = this.#info.leds.animations[this.#info.leds.play.index].settings[this.#editing]
    const modelVal = this.#animationColorValue(this.#editing)
    setting.palette = 0
    setting.color = number
    setting.rgb = [...modelVal.rgb]
    this.#refreshSelectPaletteList()
    this.#refreshSelectColor()

    const patch = {
      index: this.#info.leds.play.index
    }
    patch[this.#editing] = setting
    this.#throttledSaveAnimationSettings(patch)
  }

  /**
   * Refresh the animation options page
   */
  #refreshAnimationOptions() {
    const animations = this.#info.leds.animations[this.#info.leds.play.index]
    const list = document.querySelector('.page.options .list')
    let html = ''
    let fields = []
    for (let field of this.#globalFields) {
      fields.push(field.id)
    }
    if (animations.fields) {
      for (let field of animations.fields) {
        fields.push(field.id)
      }
    }
    for (let field of fields) {
      html += this.#optionHtml(this.#optionsField(field))
    }
    list.innerHTML = html
    for (let field of fields) {
      this.#refreshOptionControls(this.#optionsField(field))
    }
  }
  
  /**
   * Get the color object associated with the current animation field
   * @param {string} id - Field id
   * @returns An object {color, rgb}
   */
  #animationColorValue(id) {
    const val = structuredClone(this.#info.leds.animations[this.#info.leds.play.index].settings[id])
    if (val.color && val.color <= this.#info.leds.colors.length) {
      val.rgb = this.#info.leds.colors[val.color-1]
    }
    return val
  }

  /**
   * Refresh the active tab
   */
  #refreshSelectColorActiveTab() {
    document.querySelectorAll('#pAnimationColor .tabs .tab').forEach(element => {
      element.classList.remove('selected')
      if (element.dataset.tab == this.#activeTab) {
        element.classList.add('selected')
      }
    })
    document.querySelectorAll('#pAnimationColor .page').forEach(element => {
      element.style.display = element.dataset.page == this.#activeTab ? '' : 'none'
    })
  }

  /**
   * Refresh the page to select an animation palette
   */
  #refreshSelectPaletteList() {
    const field = this.#optionsField(this.#editing)
    const val = this.#info.leds.animations[this.#info.leds.play.index].settings[field.id]
    let html = ''
    this.#info.leds.palettes.forEach((palette, index) => {
      const classes = ['palette-color', 'color', 'flex-shrink-0']
      classes.push(val.palette - 1 == index ? 'selected' : 'selectable')
      const gradient = this.#backgroundGradient(palette)
      html += `<div onclick="app.selectPaletteNumber(this)" class="${classes.join(' ')}" style="background:${gradient}"><div class="name text-center">${palette.name}</div></div>`
    })
    document.querySelector('#pAnimationColor .palettes').innerHTML = html
  }

  /**
   * Refresh the select color page with the field being currently edited
   */
  #refreshSelectColor() {
    const field = this.#optionsField(this.#editing)
    const val = this.#animationColorValue(field.id)

    const colors = [val.rgb].concat(this.#info.leds.colors)
    let html = ''
    colors.forEach((rgb, index) => {
      const classes = ['color', 'font-huge', 'flex-grow-1']
      classes.push(!val.palette && val.color == index ? 'selected' : 'selectable')
      if (index < colors.length - 1) {
        classes.push('mr-3')
      }
      const basis = index ? 15: 25
      const label = index ? `<div class="number">${index}</div>` : ''
      const styles = ['height:100px', `background-color:${this.#toHex(rgb)}`, `flex-basis:${basis}%`]
      html += `<div onclick="app.selectColorNumber(this)" class="${classes.join(' ')}" style="${styles.join(';')}">${label}</div>`
    })
    document.querySelector('#pAnimationColor .colors').innerHTML = html
    this.#refreshSelectColorControls(field, val)
  }

  /**
   * Refresh the controls for color selection
   * @param {object} field - The field being edited
   * @param {object} val - The field's value
   */
  #refreshSelectColorControls(field, val) {
    const pageElement = document.querySelector('#pAnimationColor')
    const colorElement = pageElement.querySelectorAll('.colors .color')[val.color]
    this.#refreshHSVControls(field, pageElement, colorElement, val.rgb)
  }
  
  /**
   * 
   * @param {HTMLElement} itemElement - The element of the field being edited
   * @param {string} id 
   * @param {*} modelVal 
   * @param {*} val 
   */
  async #setOptionModelAndSave(itemElement, id, modelVal, val) {
    const field = this.#optionsField(id)
    const isGlobal = this.#globalFields.some(f => f.id == id)
    if (isGlobal) {
      this.#info.leds.play.settings[id] = modelVal
    } else {
      this.#info.leds.animations[this.#info.leds.play.index].settings[id] = modelVal
    }
    this.#refreshOptionValue(itemElement, val, field)
    const patch = {}
    patch[id] = modelVal
    if (isGlobal) {
      this.#throttledSavePlaySettings(patch)
    } else {
      patch.index = this.#info.leds.play.index
      this.#throttledSaveAnimationSettings(patch)
    }
  }

  /**
   * Save the play options. Called from a throttled function to prevent overloading the server with
   * too frequent calls while a user is adjusting sliders
   * @param {object} patch - An object of key = values passed to the server
   */
  #savePlaySettings(patch) {
    fetch('/api/leds/play/settings', {
      method:'PATCH',
      body:JSON.stringify(patch)
    })
  }

  /**
   * Save the animation options. Called from a throttled function to prevent overloading the server with
   * too frequent calls while a user is adjusting sliders
   * @param {object} patch - An object of key = values passed to the server
   */
  #saveAnimationSettings(patch) {
    fetch('/api/leds/animations/settings', {
      method:'PATCH',
      body:JSON.stringify(patch)
    })
  }

  /**
   * Get the field information for the specified id from either the global fields or the animation's fields
   * @param {string} id - The field id
   */
  #optionsField(id) {
    return this.#globalFields.find(f => f.id == id)|| this.#info.leds.animations[this.#info.leds.play.index].fields?.find( f=> f.id == id)
  }

  /**
   * Get the HTML for the specified field using the control templates in HTML scripts
   * @param {object} field - Field information
   */
  #optionHtml(field) {
    const id = field.id
    const label = field.label

    const element = document.getElementById(`t_${field.type}`)
    switch (field.type) {
      case 'slider': {
        const range = [field.min, field.max]
        const rangeLabels = []
        range.forEach(val => {
          rangeLabels.push(field.template ? eval(field.template, val) : val)
        })
        return eval('`' + element.innerHTML + '`', id, label, range, rangeLabels)    
      }
      case 'hue':
      case 'color':
        return eval('`' + element.innerHTML + '`', id, label)
      default:
        return ''
    }
  }

  /**
   * Refresh the controls for the specified field using the play or animation model settings
   * @param {object} field - Field information
   */
  #refreshOptionControls(field) {
    let val = this.#info.leds.play.settings[field.id] || this.#info.leds.animations[this.#info.leds.play.index].settings[field.id]

    const itemElement = document.querySelector(`.page.options [data-field="${field.id}"]`)
    switch (field.type) {
      case 'slider':
      case 'hue':
        if (field.factor) val /= field.factor
        itemElement.querySelector('input').value = val
        break
      case 'color': {
        const val = this.#info.leds.animations[this.#info.leds.play.index].settings[field.id]
        itemElement.querySelectorAll('.color-controls > div').forEach(element => {
          const isPaletteControl = element.dataset.palette !== undefined

          element.style.display = isPaletteControl == Boolean(val.palette) ? '' : 'none'
        })
        if (val.palette) {
          let number = val.palette <= this.#info.leds.palettes.length ? val.palette - 1 : 0
          const palette = this.#info.leds.palettes[number]
          itemElement.querySelector('.palette-color').style.background = this.#backgroundGradient(palette)
          itemElement.querySelector('.palette-color .name').innerText = palette.name
        } else {
          const color = this.#animationColorValue(field.id)
          const hsv = this.#rgb2hsv(color.rgb)
          itemElement.querySelector('input').value = hsv.h
        }
      }
    }
    this.#refreshOptionValue(itemElement, val, field)
  }

  /**
   * Refresh the controls for a specific item with the value
   * @param {HTMLElement} element - Top level item option div element
   * @param {string} val - The value
   * @param {object} field - Field information
   */
  #refreshOptionValue(element, val, field) {
    if (field.decimals != undefined) {
      const multiple = Math.pow(10, field.decimals)
      val = Math.round(val*multiple)/multiple
    }
    switch (field.type) {
      case 'slider':
        if (field.template) {
          val = eval(field.template, val)
        }
        element.querySelector('.value').innerHTML = val
        break
      case 'hue':
        val *= (360/255)
        element.querySelector('.value').style.background = `hsl(${val},100%,50%)`
        break
      case 'color': {
        const valueElement = element.querySelector('.value')
        const color = this.#animationColorValue(field.id)
        if (color.color && color.color <= this.#info.leds.colors.length) {
          valueElement.innerHTML = `<div class="number">${color.color}</div>`
        } else {
          valueElement.innerHTML = ''
        }
        valueElement.style.backgroundColor = this.#toHex(color.rgb)
        break
      }
    }
  }

  #backgroundGradient(palette) {
    const stops = palette.stops
      .map(stop => `rgb(${stop[1]},${stop[2]},${stop[3]}) ${Math.round(stop[0]/.255)/10}%`)
      .join(',')
    return`linear-gradient(90deg,${stops})`
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
    this.#info.leds.animations.forEach((animation, index) => {
      const selected = index == this.#info.leds.play.index ? ' selected' : ''
      const tags = animation.tags?.split(',')?.map(v => `<div class="img img-tag-${v}"></div>`)?.join('') || ''
      html += '<div class="item selectable d-flex pa-3' + selected + '" onclick="app.onAnimationClick(' + index + ')"><div class="text">' + animation.name + '</div>' + tags + '</div>'
    })
    document.getElementById('animations').innerHTML = html
  }

  // -----------------------------------------------------------------------------
  // Settings > Led Strip page
  // -----------------------------------------------------------------------------

  /**
   * Handle save
   */
  async onStripSave() {
    const patch = {}
    ['chipset', 'colorOrder'].forEach(id => {
      const value = Number(document.getElementById(`ledStrip_${id}`).value)
      patch[id] = value
      this.#info.leds[id] = value
    })
    await fetch('/api/leds/settings',{
      method:'PATCH',
      body:JSON.stringify(patch)
    })
    this.page = 'pSettings'
  }

  /**
   * Refresh the led strip details
   */
  #refreshLedStrip() {
    ['chipset', 'colorOrder'].forEach(id => {
      let html = ''
      this.#info.leds[`${id}s`].forEach((option, index) => {
        html += `<option value="${index}">${option}</option>`
      })
      const element = document.getElementById(`ledStrip_${id}`)
      element.innerHTML = html
      element.value = this.#info.leds[id]
    })
  }

  // -----------------------------------------------------------------------------
  // Settings > Led Layout page
  // -----------------------------------------------------------------------------

  /**
   * Handle the save button click by saving the LED Layout to the server
   */
  async onLayoutSave() {
    await this.#saveLedLayout()
    this.page = 'pSettings'
  }

  /**
   * Handle a layout type change by showing the relevant options page for that LED layout
   */
  onLayoutTypeChange() {
    this.#editing.layout.type = document.getElementById('ledLayoutType').value
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
    this.#editing.layout.code = document.getElementById('code').value
    this.#refreshCodeInfo()
    this.#refreshLayoutSave()
  }
  
  /**
   * Handle a change to the fit type
   */
  onLayoutFitChange() {
    this.#editing.layout.fit = document.getElementById('ledFit').value
    this.#refreshCodeInfo()
  }

  /**
   * Convert a list of [x, y, z] coordinates into a flattened array of floating point
   * @param coordinates - A list of [x, y, z] coordinates
   */
  #coordinatesToText(coordinates) {
    return coordinates
      .flat()
      .map(v => v == 0 ? '0' : v.toFixed(3).replace(/0+$/g, '').replace(/\.$/, ''))
      .join(',')
  }

  /**
   * Clone the LED layout information from the global settings so it can be modified by controls prior to saving
   */
  #copyLedLayout() {
    this.#editing = {
      layout: {},
    }

    try {
      this.#editing.layout = JSON.parse(this.#info.leds.layout)
    } catch { }
  }

  /**
   * Refresh the controls from the model
   */
  #refreshLedLayout() {
    document.getElementById('ledCount').value = this.#info.leds.count
    const type = this.#editing.layout.type||'strip'
    document.getElementById('ledLayoutType').value = type
    document.querySelectorAll('#pLedLayout .page-layout').forEach(e => {
      e.style.display = e.dataset.page == type ? 'block' : 'none'
    })
    document.getElementById('ledFit').value = this.#editing.layout.fit||'contain'
    document.getElementById('code').value = this.#editing.layout.code||''
  }

  /**
   * Attempt to parse the text in the code input into an array of 3d coordinates.
   * If successful update the model with the coordinate info and led count
   */
  #refreshCodeInfo() {
    var parsedOk = false
    this.#editing.xyz = []
    if (this.#editing.layout.code?.trim().length) {
      try {
        const code = this.#editing.layout.code
        const pts = eval(`var x=${code};if(typeof x==='function'){x()}else{x}`)
        if (!Array.isArray(pts)) {
          throw ''
        }
        pts.forEach(pt => {
          if (Array.isArray(pt)) {
            const coordinate = [0, 0, 0]
            for (let i = 0; i < 3; i++) {
              if (i < pt.length && typeof pt[i] === 'number') {
                coordinate[i] = pt[i]
              }
            }
            this.#editing.xyz.push(coordinate)
          }
        })
        if (this.#editing.layout.fit != 'none') {
          this.#editing.xyz = this.#fitCoordinates(this.#editing.xyz, this.#editing.layout.fit != 'fill') 
        }
        parsedOk = true
      } catch (ex) {
        this.#editing.xyz = []
        console.log('Error: ' + ex)
      }
    }
    document.getElementById('codeLeds').innerHTML = parsedOk ? this.#editing.xyz.length : '&#9888;'
    const previewElement = document.getElementById('layoutPreview')
    previewElement.vertices = this.#editing.xyz
  }

  /**
   * Process the coordinates by fitting them into a cube whose dimensions are [0..1]
   * @param {Array} xyz - An array of [x,y,z] coordinates
   * @param {boolean} contain 
   *   - If true contain the points in the cube and keep the same aspect ratio
   *   - if false fill the point to the cube by stretching or shrinking each axis separately
   */
  #fitCoordinates(xyz, contain) {
    if (!xyz.length) {
      return []
    }

    const minMaxes = [
      [Infinity, -Infinity], // min[x], max[x]
      [Infinity, -Infinity], // min[y], max[y]
      [Infinity, -Infinity], // min[z], max[z]
    ]

    // Find the min and max for each axis
    for (let coord of xyz) {
      for (let i = 0; i < 3; i++) {
        minMaxes[i][0] = Math.min(minMaxes[i][0], coord[i])
        minMaxes[i][1] = Math.max(minMaxes[i][1], coord[i])
      }
    }

    const axisRanges = minMaxes.map(axisMinMax => axisMinMax[1] - axisMinMax[0])
    let scale = [1, 1, 1]

    if (contain) {
      // Find the maximum range
      const maxRange = Math.max(...axisRanges)
      scale = [1 / maxRange, 1 / maxRange, 1 / maxRange]
    } else {
      // Scale each axis independently
      scale = axisRanges.map(axisRange => 1 / axisRange)
    }

    // Normalize coordinates
    return xyz.map(
      coord => [0, 1, 2].map(
        axis => Number.isFinite(scale[axis]) ? (coord[axis] - minMaxes[axis][0]) * scale[axis] : 0.5
      )
    )
  }

  /**
   * Refresh the save button enabled state based on whether a valid LED layout has been specified.
   * For now all that is required is the total number of leds is > 0
   */
  #refreshLayoutSave() {
    let canSave = false
    switch (this.#editing.layout.type) {
      case 'code':
        canSave = this.#editing.xyz.length
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
    const patch = {}
    switch (this.#editing.layout.type) {
      case 'code':
        patch.count = this.#editing.xyz.length
        patch.layout = JSON.stringify(this.#editing.layout)
        patch.coordinates = this.#coordinatesToText(this.#editing.xyz)
        break
      default: // 'strip'
        patch.count = parseInt(document.getElementById('ledCount').value)
        const coords = []
        for (let i = 0; i < patch.count; i++) {
          const coord = (i+.5)/patch.count
          coords.push(coord + ',' + coord + ',' + coord)
        }
        patch.layout = '{"type":"strip"}',
        patch.coordinates = coords.join(',')
        break
    }
    if (patch.layout) {
      this.#info.leds.count = patch.count
      this.#info.leds.layout = patch.layout
      await fetch('/api/leds/settings',{
        method:'PATCH',
        body:JSON.stringify(patch)
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
  onEditColor(index) {
    this.#editing = index
    document.querySelector('#pColorEdit .title .text').innerText = `Color #${index+1}`
    this.#refreshColorEdit()
    this.page = 'pColorEdit'
  }

  /**
   * Handle a control change
   * @param {HTMLElement} element - The element whose value is changing
   */
  async onHsvChange(element) {
    const rgb = this.#getColorHSVControlRGB('pColorEdit')

    this.#info.leds.colors[this.#editing] = [...rgb]
    this.#refreshColorEdit(element.dataset.field)
    this.#throttledSaveColor({
      index: this.#editing,
      rgb: rgb
    })
  }

  /**
   * Handle a click in the rainbow strip below the hue slider to pick a hue
   * @param {HTMLElement} element 
   * @param {PointerEvent} event 
   */
  async onColorHueClick(element, event) {
    var hue = Math.floor(event.offsetX/element.clientWidth*255)
    var inputElement = element.closest('.item').querySelector('input')
    inputElement.value = hue
    this.onHsvChange(inputElement)
  }

  /**
   * Refresh the details of the edit color page
   * @param {object} field - The field
   */
  #refreshColorEdit(field) {
    const rgb = this.#info.leds.colors[this.#editing]
    const pageElement = document.querySelector('#pColorEdit')
    const colorElement = pageElement.querySelector('.color')
    this.#refreshHSVControls(field, pageElement, colorElement, rgb)
  }

  /**
   * Refresh a color block and hue, saturation, value sliders with a color
   * @param {object} field - The field
   * @param {HTMLElement} pageElement - The page element
   * @param {HTMLElement} colorElement - The current color element
   * @param {Array} rgb - An array of [r,g,b]
   */
  #refreshHSVControls(field, pageElement, colorElement, rgb) {
    if (colorElement) {
      colorElement.style.backgroundColor = this.#toHex(rgb)
    }
    const hsv = this.#rgb2hsv(rgb)
    pageElement.querySelectorAll('[data-type="hsv"]').forEach(element => {
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
    this.#info.leds.colors.forEach((bg, index) => {
      const bgHex = this.#toHex(bg)
      html += `<div onclick="app.onEditColor(${index})" class="color font-huge selectable" style="background-color:${bgHex};height:100px"><div class="number">${index+1}</div></div>`
    })
    document.querySelector('#pColors .grid').innerHTML = html
  }

  /**
   * Get the hue, saturation, value control values from the page and return an [r,g,b] array
   * @param {string} pageId - The page id
   */
  #getColorHSVControlRGB(pageId) {
    const hsv = {}
    document.querySelectorAll(`#${pageId} [data-type="hsv"]`).forEach(element => {
      hsv[element.dataset.field] = element.value
    })
    return this.#hsv2rgb(hsv)
  }

  /**
   * Save the information for the color
   * @param {object} patch - An object {index, rgb: [r,g,b]}
   */
  #saveColor(patch) {
    fetch('/api/leds/colors', {
      method:'PUT',
      body:JSON.stringify(patch)
    })
  }

  // -----------------------------------------------------------------------------
  // Settings > Palettes page
  // -----------------------------------------------------------------------------

  /**
   * Handle click to edit a palette
   * @param index 
   */
  onEditPalette(index) {
    this.#editing = {
      palette: index
    }
    document.querySelector('#pPaletteEdit .title .text').innerText = this.#info.leds.palettes[index].name
    this.#refreshPaletteEdit()
    this.page = 'pPaletteEdit'
    document.getElementById('paletteName').focus();
  }
  
  /**
   * Delete the selected palette
   */
  onPaleteDelete() {
    // TODO: Implement delete palette
  }

  /**
   * Handle name change events
   */
  onPaletteNameChange() {
    const palette = this.#info.leds.palettes[this.#editing.palette]
    const name = document.getElementById('paletteName').value
    document.querySelector('#pPaletteEdit .title .text').innerText = name
    palette.name = name
    this.#throttledSavePalette({
      index: this.#editing.palette,
      name: palette.name,
      stops: palette.stops
    })
  }

  /**
   * Add a stop
   */
  onAddStop() {
    // TODO: Implement add stop
  }

  /**
   * Remove the stop at the specified index
   * @param {PointerEvent} event The click event
   * @param {number} index The index
   */
  onDeleteStop(event, index) {
    event.stopPropagation()
    const palette = this.#info.leds.palettes[this.#editing.palette]
    palette.stops.splice(index, 1)
    this.#refreshGradient()
    this.#refreshPaletteStops()
    this.#saveCurrentPalette()
  }

  /**
   * Edit the stop
   * @param {number|undefined} index - The stop index to edit
   */
  onEditStop(index) {
    this.#editing.stop = this.#info.leds.palettes[this.#editing.palette].stops[index]
    this.#refreshPaletteStops()
  }

  /**
   * Handle the stop percent slider change
   * @param {HTMLElement} element The element
   */
  onStopPercentChange(element) {
    const palette = this.#info.leds.palettes[this.#editing.palette]
    this.#editing.stop[0] = parseInt(element.value)
    palette.stops.sort((a, b) => a[0] - b[0])

    this.#refreshGradient()
    this.#refreshPaletteStops()
    this.#saveCurrentPalette()
  }

  /**
   * Handle a stop HSV slider change
   * @param {HTMLElement} element The element whose value is changing
   */
  onStopHsvChange(element) {
    const palette = this.#info.leds.palettes[this.#editing.palette]
    const rgb = this.#getColorHSVControlRGB('pPaletteEdit')

    for(let i = 0; i < 3; i++) {
      this.#editing.stop[i+1] = rgb[i]
    }
    this.#refreshGradient()
    this.#refreshPaletteStops(element.dataset.field)
    this.#saveCurrentPalette()
  }

  /**
   * Handle a click in the rainbow strip below the hue slider to pick a hue for a palette
   * @param {HTMLElement} element 
   * @param {PointerEvent} event 
   */
  async onStopHueClick(element, event) {
    var hue = Math.floor(event.offsetX/element.clientWidth*255)
    var inputElement = element.closest('.item').querySelector('input')
    inputElement.value = hue
    this.onStopHsvChange(inputElement)
  }

  /**
   * Refresh the palette edit page
   */
  #refreshPaletteEdit() {
    const palette = this.#info.leds.palettes[this.#editing.palette]
    document.querySelector('#pPaletteEdit .btn-delete').style.display = this.#editing.palette ? '' : 'none'
    document.getElementById('paletteName').value = palette.name
    this.#refreshGradient()
    this.#refreshPaletteStops()
  }

  /**
   * Refresh the palette gradient
   */
  #refreshGradient() {
    const palette = this.#info.leds.palettes[this.#editing.palette]
    document.getElementById('paletteGradient').style.background = this.#backgroundGradient(palette)
  }

  /**
   * Refresh the stops panel
   */
  #refreshPaletteStops(field) {
    const palette = this.#info.leds.palettes[this.#editing.palette]
    const stop = this.#editing.stop

    const stopsElement = document.getElementById('stops')
    const stopElement = document.getElementById('stop')
    const showHide = Array.from(document.querySelector('#stopName').children).concat([stopsElement, stopElement])
    showHide.forEach(element => {
      element.style.display = (stop && 'stop' in element.dataset) || (!stop && !('stop' in element.dataset)) ? '' : 'none'
    })

    if (stop) {
      const rgb = [stop[1], stop[2], stop[3]]
      document.querySelector('#stopName .stop-color').style.backgroundColor = this.#toHex(rgb)
      const percentElement = document.querySelector('#stop [data-field="percent')
      if (field != 'percent') {
        percentElement.value = stop[0]
      }
      percentElement.closest('.item').querySelector('.value').innerText = String(this.#editing.stop[0]).padStart(3, '0')
      const hsv = this.#rgb2hsv(rgb)
      document.querySelectorAll('#stop [data-type="hsv"]').forEach(element => {
        if (element.dataset.field != field) {
          element.value = hsv[element.dataset.field]
        }
      })
    } else {
      const template = document.getElementById('t_stop')
      let html = ''
      palette.stops.forEach((stop, index) => {
        const position = String(stop[0]).padStart(3, '0')
        const rgb = this.#toHex([stop[1], stop[2], stop[3]])
        let btn = ''
        if (palette.stops.length > 2) {
          btn = `<div onclick="app.onDeleteStop(event, ${index})" class="btn flex-shrink-0 rounded pa-3"><div class="img img-close"></div></div>`
        }
        html += eval('`' + template.innerHTML + '`', index, position, rgb, btn)    
      })
      stopsElement.innerHTML = html  
    }
  }

  /**
   * Refresh the palettes page
   */
  #refreshPalettes() {
    let html = ''
    this.#info.leds.palettes.forEach((palette, index) => {
      const gradient = this.#backgroundGradient(palette)
      html += `<div onclick="app.onEditPalette(${index})" class="palette-color color selectable" style="background:${gradient}"><div class="name text-center">${palette.name}</div></div>`
    })
    document.querySelector('#pPalettes .page').innerHTML = html
  }

  #saveCurrentPalette() {
    const palette = this.#info.leds.palettes[this.#editing.palette]
    this.#throttledSavePalette({
      index: this.#editing.palette,
      name: palette.name,
      stops: palette.stops
    })
  }

  #savePalette(patch) {
    fetch('/api/leds/palettes', {
      method:'PUT',
      body:JSON.stringify(patch)
    })
  }

  // -----------------------------------------------------------------------------
  // About page
  // -----------------------------------------------------------------------------

  /**
   * Query the server for the system info, update the UI, and set a timer to check again in 2 seconds
   */
  async #updateSystemInfo() {
    let response = { ok: false }
    try {
      response = await fetch('/api/system')
    } catch {}
    if (response.ok) {
      const systemInfo = await response.json()
      let html = ''
      for (const [sectionName, sectionValue] of Object.entries(systemInfo)) {
        let sectionHtml = ''
        for (let [key, value] of Object.entries(sectionValue)) {
          if (!isNaN(value)) {
            value = Math.round(value).toLocaleString()
          }
          sectionHtml += '<div>' + key + '</div><div class="text-right">' + value + '</div>'
        }
        html += '<h2>' + sectionName + '</h2><div class="d-grid w-100" style="grid-template-columns: 1fr 1fr;gap: 12px">' + sectionHtml + '</div>'
      }
      document.querySelector('#pAbout .page').innerHTML = html
    } else {
      document.querySelector('#pAbout .page').innerText = 'unavailable'
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

 
  /**
   * Convert an rgb array to a hex string
   * @param {Array} rgb - An array of [r,g,b]
   */
  #toHex(rgb) {
    let hex = '#';
    rgb.forEach(v => {
      hex += v.toString(16).padStart(2,'0')
    })
    return hex
  }
  
  /**
   * Convert an rgb array to a hue, saturation, value
   * @param {Array} rgb - An array of [r,g,b]
   * @returns A structure of {h, s, v} whose values are [0..255]
   */
  #rgb2hsv(rgb) {
    const r = rgb[0]/255, g = rgb[1]/255, b = rgb[2]/255
    const v = Math.max(r, g, b), c = v-Math.min(r, g, b)
    const h = c && ((v == r) ? (g-b)/c : ((v == g) ? 2 + (b-r)/c : 4+(r-g)/c))
    return {
      h: Math.round(42.5*(h < 0 ? h+6 : h)),
      s: Math.round(255*(v&&c/v)),
      v: Math.round(255*v)
    }
  }
  
  /**
   * Convert a hue, saturation, value to red, green, blue
   * @param hsv - A structure of {h, s, v} whose values are [0..255]
   * @returns - An array of [r,g,b]
   */
  #hsv2rgb(hsv) {
    const h = hsv.h*360/255, s = hsv.s/255, v = hsv.v/255
    const f = (n,k=(n+h/60)%6) => v - v*s*Math.max( Math.min(k,4-k,1), 0)
    return [
      Math.round(255*f(5)),
      Math.round(255*f(3)),
      Math.round(255*f(1))
    ]
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
   * Active tab
   */
  #activeTab
  /**
   * Model object for data while it's being edited
   */
  #editing
  /**
   * A throttled save play setting function. This prevents sliders in the UI from calling the backend too frequently with updates
   */
  #throttledSavePlaySettings
  /**
   * A throttled save animation function. This prevents sliders in the UI from calling the backend too frequently with updates
   */
  #throttledSaveAnimationSettings
  /**
   * A throttled save color function. This prevents sliders in the UI from calling the backend too frequently with updates
   */
  #throttledSaveColor
  /**
   * A throttled save palette function. This prevents sliders in the UI from calling the backend too frequently with updates
   */
  #throttledSavePalette
  /**
   * A javascript interval id for periodically freshing the play preview image
   */
  #playRefreshInterval
  /**
   * A javascript interval id for periodically freshing a UI element from the server (such as system info)
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