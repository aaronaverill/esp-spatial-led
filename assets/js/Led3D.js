class Quaternion {
  constructor(w, x, y, z) {
    this.w = w
    this.x = x
    this.y = y
    this.z = z
  }

  static multiply(q1, q2) {
    return new Quaternion(
      q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
      q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
      q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
      q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w
    )
  }

  static rotatePoint(q, point) {
    const p = new Quaternion(0, point[0], point[1], point[2])
    const qConjugate = new Quaternion(q.w, -q.x, -q.y, -q.z)
    const rotatedP = Quaternion.multiply(Quaternion.multiply(q, p), qConjugate)
    return [rotatedP.x, rotatedP.y, rotatedP.z ]
  }

  static fromAxisAngle(axis, angle) {
    const halfAngle = angle / 2
    const s = Math.sin(halfAngle)
    return new Quaternion(Math.cos(halfAngle), axis[0] * s, axis[1] * s, axis[2] * s)
  }

  static fromEulerAngles(xAngle, yAngle, zAngle) {
    const xQuat = Quaternion.fromAxisAngle([1, 0, 0], xAngle)
    const yQuat = Quaternion.fromAxisAngle([0, 1, 0], yAngle)
    const zQuat = Quaternion.fromAxisAngle([0, 0, 1], zAngle)

    // Combined rotation quaternion
    return Quaternion.multiply(Quaternion.multiply(zQuat, yQuat), xQuat)
  }
}

/**
 * Web component to display layout mapping in a 3d view
 */
export default class Led3D extends HTMLElement {
  constructor() {
    super()

    // Attach a shadow DOM
    this.attachShadow({ mode: 'open' })

    // Create a canvas element
    this.#canvas = document.createElement('canvas')
    this.shadowRoot.appendChild(this.#canvas)

    // Bind the resize event
    this.#resizeObserver = new ResizeObserver(() => this.#onResize())
    this.#resizeObserver.observe(this)

    // Mouse events
    this.addEventListener('mousedown', this.#handleMouseDown.bind(this))
    this.addEventListener('mousemove', this.#handleMouseMove.bind(this))
    this.addEventListener('mouseup', this.#handleMouseUp.bind(this))
    this.addEventListener('wheel', this.#handleMouseWheel.bind(this))

      // Animation loop
    requestAnimationFrame((timestamp) => { this.#animate(timestamp) })
  }

  /**
   * Set the vertices
   * @param {Array} vertices - An array of [x,y,z] tuples
   */
  set vertices(vertices) {
    this.#vertices = []
    if (vertices.length) {
      // Scale the values so they fit within a [-1, 1] cube centered on (0,0)
      let minValues = [Infinity, Infinity, Infinity]
      let maxValues = [-Infinity, -Infinity, -Infinity]

      for (let vertex of vertices) {
        for (let i = 0; i < 3; i++) {
          if (vertex[i] < minValues[i]) minValues[i] = vertex[i]
          if (vertex[i] > maxValues[i]) maxValues[i] = vertex[i]
        }
      }

      const ranges = maxValues.map((max, i) => max - minValues[i])
      const maxRange = Math.max(...ranges)
      const scaleFactor = 2 / maxRange

      const centers = maxValues.map((max, i) => (max + minValues[i]) / 2)

      this.#vertices = vertices
        .map(vertex => vertex
          .map((value, i) => isFinite(scaleFactor) ? (value - centers[i]) * scaleFactor : 0.5) 
        )
    }
    this.#render()
  }

  /**
   * Set the color array
   * @param {ArrayBuffer} colorArray - A ArrayBuffer object with R,G,B bytes for the colors
   */
  set colorArray(colorArray) {
    this.#colorArray = colorArray
    this.#render()
  }

  /**
   * Render the vertex information to the canvas
   */
  #render() {
    // Set canvas size
    const rect = this.getBoundingClientRect()
    if (!rect.width || !rect.height) {
      return
    }

    this.#canvas.width = rect.width
    this.#canvas.height = Math.floor(rect.height)

    const drawRect = {
      x: 0, 
      y: 0, 
      w: rect.width, 
      h: rect.height
    }
    const points = this.#getCoordinateDrawInfo(this.#vertices, drawRect, this.#zoom, new Uint8Array(this.#colorArray))

    const ctx = this.#canvas.getContext('2d')
    // Fill the background
    ctx.fillStyle = '#181818' // Background color
    ctx.fillRect(0, 0, this.#canvas.width, this.#canvas.height)

    // Draw the wires
    ctx.lineWidth = 1
    points.forEach((point, index) => {
      if (index > 0) {
        const lineAlpha = 0.25 * (points[index-1].color[3] + point.color[3])
        ctx.beginPath()
        ctx.strokeStyle = `rgba(255,255,255,${lineAlpha})`
        ctx.moveTo(points[index-1].x, points[index-1].y)
        ctx.lineTo(point.x, point.y)
        ctx.stroke()
      }
    })

    // Sort the leds from back to front so closer points are drawn on top
    points.sort((a, b) => b.depth - a.depth)

    // Draw the leds
    points.forEach(point => {
      const radius = this.#radius * (1 - point.depth * .5) * this.#zoom
      ctx.beginPath()
      ctx.fillStyle = `rgba(${point.color[0]},${point.color[1]},${point.color[2]},${point.color[3]})`
      ctx.arc(point.x, point.y, radius, 0, 2 * Math.PI)
      ctx.fill()
      ctx.lineWidth = radius * 2
      ctx.strokeStyle = `rgba(${point.color[0]},${point.color[1]},${point.color[2]},${point.color[3]*.3})`
      ctx.stroke()
    })

    // Draw dout and din
    if (points.length) {
      const firstPoint = points.find(p => p.i == 0)
      this.#drawText(ctx, 'din', firstPoint.x, firstPoint.y - this.#radius - 10, firstPoint.color[3])
      if (points.length > 1) {
        const lastPoint = points.find(p => p.i ==points.length - 1)
        this.#drawText(ctx, 'dout', lastPoint.x, lastPoint.y - this.#radius - 10, lastPoint.color[3])
      }
    }

    // Draw the axis
    const axisCoordinates = []
    for(let i = 0; i < 7; i++) {
      axisCoordinates[i] = [0, 0, 0]
    }
    for(let i = 0; i < 3; i++) {
      axisCoordinates[i][i] = 1
      axisCoordinates[3+i][i] = 0.8
    }

    const axisSize = 100
    const axisRect = {
      x: 5,
      y: drawRect.h - axisSize - 5,
      w: axisSize,
      h: axisSize
    }
    const axisPoints = this.#getCoordinateDrawInfo(axisCoordinates, axisRect, 0.6)
    const axisColors = ['#F66', '#66F', '#8E8']
    const axisLabels = ['x', 'y', 'z']

    ctx.font = '14pt Arial'
    ctx.textBaseline = 'middle'
    ctx.textAlign = 'center'
    ctx.lineWidth = 3
    axisColors.forEach((color, index) => {
      ctx.strokeStyle = color
      ctx.beginPath()
      ctx.moveTo(axisPoints[6].x, axisPoints[6].y)
      ctx.lineTo(axisPoints[3+index].x, axisPoints[3+index].y)
      ctx.stroke()

      ctx.fillStyle = color
      ctx.fillText(axisLabels[index], axisPoints[index].x, axisPoints[index].y)
    })


    // Draw a nice border
    ctx.strokeStyle = '#666'  // Border color
    ctx.lineWidth = 1
    ctx.strokeRect(0, 0, this.#canvas.width, this.#canvas.height)
  }

  /**
   * 
   * @param {Array} coordinates - An array of coordinates in [x,y,z] format
   * @param {object} drawRect - An object where the coordinates are drawn {x,y,w,h}
   * @param {number} zoom - The zoom factor, use 0.33 to fit a [0..1] cube
   * @param {ArrayByffer} colors - The coordinate colors in r,g,b order
   * @returns 
   */
  #getCoordinateDrawInfo(coordinates, drawRect, zoom, colors) {
    // Center of the draw area
    const center = [
      drawRect.x + drawRect.w / 2,
      drawRect.y + drawRect.h / 2,
      0
    ]
    const scale = zoom * Math.min(drawRect.w, drawRect.h)
    
    const points = []
    coordinates.forEach((coordinate, index) => {
      const xyz = Quaternion.rotatePoint(this.#rotation, coordinate)

      const depth = 0.5 - xyz[2] / 3.4645 // 0 (closest) to 1 (farthest)

      // Project onto 2D screen
      const depthScale = 1 - depth * 0.5
      const screenXY = xyz.map((value, i) => center[i] + value * depthScale * scale)

      // Determine color
      const colorIndex = index * 3
      const r = colors && colorIndex < colors.length ? colors[colorIndex] : 255
      const g = colors && colorIndex + 1 < colors.length ? colors[colorIndex + 1] : 255
      const b = colors && colorIndex + 2 < colors.length ? colors[colorIndex + 2] : 255
      const alpha = Math.min(1, 1.3 - depth)

      // Add the LED to the points collection.
      points.push({
        i: index,
        x: screenXY[0],
        y: screenXY[1],
        color: [r, g, b, alpha],
        depth: depth
      })
    })
    return points
  }

  /**
   * Draw text above the specified point
   * @param {CanvasRenderingContext2D} ctx - Canvas context
   * @param text - The text
   * @param x - X position in client coordinates
   * @param y - Y position in client coordinates
   * @param alpha - Alpha between 0..1
   */
  #drawText(ctx, text, x, y, alpha) {
    ctx.font = '14pt Arial'
    ctx.textAlign = 'center'
    ctx.fillStyle = `rgba(255,255,255,${alpha})`
    ctx.fillText(text, x, y)
  }

  /**
   * Handle resize events
   */
  #onResize() {
    this.#render()
  }

  /**
   * Animation loop to update auto rotation and render the canvas
   * @param timestamp Time stamp of the animation event
   */
  #animate(timestamp) {
    const deltaTime = timestamp - this.#lastFrameTime

    // Update the last frame time
    this.#lastFrameTime = timestamp

    this.#render()
    if (deltaTime > 0 && this.#autoRotate) {
      const rotationQuatX = Quaternion.fromAxisAngle([0, 1, 0], this.#rotateSpeed * deltaTime/1000)
      this.#rotation = Quaternion.multiply(
        rotationQuatX,
        this.#rotation
      )
    }
    
    requestAnimationFrame((timestamp) => { this.#animate(timestamp) })
  }

  /**
   * Handle the mouse down event by disabling auto rotation
   * @param {MouseEvent} event 
   */
  #handleMouseDown(event) {
    this.#isDragging = 1
    this.#autoRotate = false
    this.#mouseDownLocation = [event.clientX, event.clientY]
    this.#mouseDownRotation = new Quaternion(this.#rotation.w, this.#rotation.x, this.#rotation.y, this.#rotation.z)
  }

  /**
   * Handle the mouse drag event by changing the Y and X angles
   * @param {MouseEvent} event 
   */
  #handleMouseMove(event) {
    if (this.#isDragging) {
      if (this.#moveDistance(event) > 6) {
        this.#isDragging = 2
      }
      if (this.#isDragging == 2) {
        const delta = [event.clientX - this.#mouseDownLocation[0], event.clientY - this.#mouseDownLocation[1]]
        const rotationQuatX = Quaternion.fromAxisAngle([1, 0, 0], -delta[1]/100)
        const rotationQuatY = Quaternion.fromAxisAngle([0, 1, 0], delta[0]/100)

        const rotationQuat = Quaternion.multiply(rotationQuatY, rotationQuatX)

        this.#rotation = Quaternion.multiply(
          rotationQuat,
          this.#mouseDownRotation
        )
        this.#render()
      }
    }
  }

  /**
   * Handle the mouse up event by turning auto rotation on
   * @param {MouseEvent} event 
   */
  #handleMouseUp(event) {
    this.#isDragging = 0
    if (this.#moveDistance(event) < 6) {
      this.#autoRotate = true
    }
  }

  #moveDistance(event) {
    const delta = [event.clientX - this.#mouseDownLocation[0], event.clientY - this.#mouseDownLocation[1]]
    return Math.sqrt(delta[0]*delta[0] + delta[1]*delta[1])
  }

  /**
   * Handle the mouse wheel event by zooming in or out
   * @param {WheelEvent} event The wheel event
   */
  #handleMouseWheel(event) {
    if (event.deltaY > 0) {
      this.#zoom *= 0.95
    } else {
      this.#zoom /= 0.95
    }
    this.#render()
  }

  /**
   * Clean up observer when element is removed 
   */
  disconnectedCallback() {
    this.#resizeObserver.disconnect()
  }

  #vertices = []
  #colorArray = new Uint8Array()

  #radius = 30
  #zoom = 0.33
  #autoRotate = true
  #rotateSpeed = -0.4
  #rotation = Quaternion.fromEulerAngles(Math.PI*3/4, Math.PI/8, -Math.PI/8)
  #isDragging = 0
  #mouseDownLocation
  #mouseDownRotation

  #lastFrameTime

  //Quaternion {w: -0.27178155050789454, x: -0.8857626750332447, y: -0.10754572916847172, z: -0.3605456815149862}

  /**
   * The canvas object
   */
  #canvas
  /**
   * Resize observer
   */
  #resizeObserver
}

/**
 * Define the custom element for this web component
 */
customElements.define('led-3d', Led3D)