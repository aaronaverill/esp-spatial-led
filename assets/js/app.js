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
    this.#refreshColors()
  
    this.#playRefreshInterval = setInterval(this.#refreshPlayPreview, 500)
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

  /**
   * Query the server for the currently playing R,G,B values and update the preview bitmap and visualization color arrays
   */
  async #refreshPlayPreview() {
    let response = { ok: false }
    try {
      response = await fetch('/api/leds/play/rgb')
    } catch { }
    if (response.ok) {
      const rgbBmpData = await response.arrayBuffer()

      // Get the header size from the BMP data starting at byte 10 stored in little endian
      const headerSize = new DataView(rgbBmpData).getUint32(10, true)
      // Update the layout preview component with the color data
      document.getElementById('layoutPreview').colorArray = rgbBmpData.slice(headerSize)

      // To display the R,G,B data as a bitmap it must be convered from R,G,B to B,G,R which
      // is the correct bitmap file format pixel order. Dumb.
      const bmpData = new ArrayBuffer(rgbBmpData.byteLength)
      const rgbView = new Uint8Array(rgbBmpData)
      const bmpView = new Uint8Array(bmpData)

      // Copy the header bytes as is
      bmpView.set(rgbView.slice(0, headerSize), 0)

      // Reorder the remaining bytes from RGB to BGR
      for (let i = headerSize; i < rgbView.length; i += 3) {
        bmpView[i] = rgbView[i + 2] // Blue
        bmpView[i + 1] = rgbView[i + 1] // Green
        bmpView[i + 2] = rgbView[i] // Red
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
   * Create the controls for the options page from the currently running animation and make the page visible
   */
  showOptions() {
    this.#refreshAnimationOptions()
    this.showPage('pPlayOptions')
  }

  /**
   * Handle the event when an option value changes
   * @param {HTMLElement} element - The element that is changing
   */
  async onOptionChange(element) {
    const itemElement = element.closest('.item')
    const id = itemElement.dataset.field
    const field = this.#optionsField(id)
    const val = element.value
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
      number: 0,
      rgb: this.#hsv2rgb({h: hue, s: 255, v: 255})
    }
    await this.#setOptionModelAndSave(itemElement, id, modelVal, modelVal)
  }


  /**
   * Handle a control change
   * @param {HTMLElement} element - The element whose value is changing
   */
  async onAnimationColorChange(element) {
    const rgb = this.#getColorHSVControlRGB('pAnimationColor')
    const number = this.#info.leds.animations[this.#info.leds.play.index].settings[this.#editing].number

    this.#info.leds.animations[this.#info.leds.play.index].settings[this.#editing].rgb = [...rgb]

    const isNumberedColor = number && number <= this.#info.leds.colors.length
    if (isNumberedColor) {
      this.#info.leds.colors[number-1] = [...rgb]
    }

    this.#refreshSelectColor()
    this.#refreshAnimationOptions()
    this.#refreshColors()
  
    if (isNumberedColor) {
      this.#throttledSaveColor({
        index: number-1,
        rgb: rgb
      })
    } else {
      const patch = {
        index: this.#info.leds.play.index,
      }
      patch[this.#editing] = {
        number: number,
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
   * Show the color selection page
   * @param {HTMLElement} element - The field's element which was clicked
   */
  showAnimationSelectColor(element) {
    const itemElement = element.closest('.item')
    this.#editing = itemElement.dataset.field
    const field = this.#optionsField(this.#editing)
    document.querySelector('#pAnimationColor .title .text').innerText = `Select ${field.label}`
    this.#refreshSelectColor()
    this.showPage('pAnimationColor')
  }

  /**
   * Set the color number to the selected element
   * @param {HTMLElement} element - Color number selected by index
   */
  selectColorNumber(element) {
    const number = Array.from(element.parentNode.children).indexOf(element)
    this.#info.leds.animations[this.#info.leds.play.index].settings[this.#editing].number = number
    const modelVal = this.#animationColorValue(this.#editing)
    this.#info.leds.animations[this.#info.leds.play.index].settings[this.#editing].rgb = [...modelVal.rgb]
    this.#refreshSelectColor()
    this.#refreshAnimationOptions()

    const patch = {
      index: this.#info.leds.play.index
    }
    patch[this.#editing] = modelVal
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
   * @returns An object {number, rgb}
   */
  #animationColorValue(id) {
    const val = structuredClone(this.#info.leds.animations[this.#info.leds.play.index].settings[id])
    if (val.number && val.number <= this.#info.leds.colors.length) {
      val.rgb = this.#info.leds.colors[val.number-1]
    }
    return val
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
      classes.push(val.number == index ? 'selected' : ' selectable')
      if (index < colors.length - 1) {
        classes.push('mr-3')
      }
      const basis = index ? 15: 25
      const label = index ? `<div>${index}</div>` : ''
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
    const colorElement = pageElement.querySelectorAll('.color')[val.number]
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
      case 'color':
        const color = this.#animationColorValue(field.id)
        const hsv = this.#rgb2hsv(color.rgb)
        itemElement.querySelector('input').value = hsv.h
        break
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
        element.querySelector('.value').style.backgroundColor = `hsl(${val},100%,50%)`
        break
      case 'color': {
        const valueElement = element.querySelector('.value')
        const color = this.#animationColorValue(field.id)
        if (color.number && color.number <= this.#info.leds.colors.length) {
          valueElement.innerHTML = `<div>${color.number}</div>`
        } else {
          valueElement.innerHTML = ''
        }
        valueElement.style.backgroundColor = this.#toHex(color.rgb)
        break
      }
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
    for (let i = 0; i < this.#info.leds.animations.length; i++) {
      const selected = i == this.#info.leds.play.index ? ' selected' : ''
      html += '<div class="item selectable pa-3' + selected + '" onclick="app.onAnimationClick(' + i + ')"><div class="text">' + this.#info.leds.animations[i].name + '</div></div>'
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
      .map(v => v == 0 ? '0' : v.toFixed(3).replace(/0+$/g, '')
      .replace(/\.$/, ''))
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
    return xyz.map(coord => [
      (coord[0] - minMaxes[0][0]) * scale[0],
      (coord[1] - minMaxes[1][0]) * scale[1],
      (coord[2] - minMaxes[2][0]) * scale[2]
    ])
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
    const rgb = this.#getColorHSVControlRGB('pColorEdit')

    this.#info.leds.colors[this.#editing] = [...rgb]
    this.#refreshColorEdit(element.dataset.field)
    this.#refreshColors()
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
    this.onColorChange(inputElement)
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
    let index = 0
    this.#info.leds.colors.forEach(bg => {
      const bgHex = this.#toHex(bg)
      html += `<div onclick="app.onColorClick(${index})" class="color font-huge selectable" style="background-color:${bgHex};height:100px"><div>${index+1}</div></div>`
      index++
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
      const fps = await response.text()
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