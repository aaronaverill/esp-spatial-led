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
    this.#resizeObserver = new ResizeObserver(() => this.#onResize());
    this.#resizeObserver.observe(this);

      // Animation loop
    requestAnimationFrame((timestamp) => { this.#animate(timestamp) })
  }

  /**
   * Set the vertices
   * @param vertices - An array of [x,y,z] tuples
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

      this.#vertices = vertices.map(vertex => vertex.map((value, i) => (value - centers[i]) * scaleFactor))
    }
    this.#render()
  }

  set colorArray(colorArray) {
    this.#colorArray = colorArray
    this.#render()
  }

  // Render method to draw on the canvas
  #render() {
    const ctx = this.#canvas.getContext('2d')
    // Set canvas size
    const rect = this.getBoundingClientRect()
    this.#canvas.width = rect.width
    this.#canvas.height = Math.floor(rect.height)

    // Fill the background with black
    ctx.fillStyle = '#222'
    ctx.fillRect(0, 0, this.#canvas.width, this.#canvas.height)

    // Center of the canvas
    const center = [
      this.#canvas.width / 2,
      this.#canvas.height / 2,
      0
    ]

    const scale = this.#zoom * Math.min(center[0], center[1])

    let lastPoint = undefined
    if (this.#vertices.length > 0) {
      const colors = new Uint8Array(this.#colorArray);
      const points = []
      this.#vertices.forEach((vertex, index) => {
        // Apply rotations
        const cos = []
        const sin = []
        for (let i = 0; i < 3; i++) {
          cos.push(Math.cos(this.#rotateAngle[i]))
          sin.push(Math.sin(this.#rotateAngle[i]))
        }

        // Rotate around each axis. Here be dragons :)
        const xyz = [...vertex]
        const axisPairs = [[1,2], [0, 2], [0, 1]]
        axisPairs.forEach((axes, axisIndex) => {
          let rotated = [
            xyz[axes[0]] * cos[axisIndex] - xyz[axes[1]] * sin[axisIndex],
            xyz[axes[0]] * sin[axisIndex] + xyz[axes[1]] * cos[axisIndex]
          ]
          for (let i = 0; i < 2; i++) {
            xyz[axes[i]] = rotated[i]
            xyz[axes[i]] = rotated[i]
          }
        })

        // Project onto 2D screen
        const depthScale = 10 / (10 - (1+xyz[2]))
        const screenXY = xyz.map((value, i) => center[i] + value * depthScale * scale)

        // TODO: Implement code to get r,g,b from color array
        // Determine color
        const colorIndex = index * 3
        const alpha = Math.min(1, 0.3+(2+xyz[2])/3)
        const r = colorIndex < colors.length ? colors[colorIndex] : 255
        const g = colorIndex + 1 < colors.length ? colors[colorIndex + 1] : 255
        const b = colorIndex + 2 < colors.length ? colors[colorIndex + 2] : 255
        const sizeScale = 6 / (6 - (1+xyz[2]))

        points.push({
          x: screenXY[0],
          y: screenXY[1],
          z: xyz[2],
          radius: this.#radius * sizeScale,
          color: [r, g, b, alpha]
        })
        if (lastPoint) {
          ctx.beginPath()
          ctx.lineWidth = 1
          ctx.strokeStyle = `rgba(255, 255, 255, ${(lastPoint[2] + alpha)/4})`
          ctx.moveTo(lastPoint[0], lastPoint[1])
          ctx.lineTo(screenXY[0], screenXY[1])
          ctx.stroke()
        }
        lastPoint = [screenXY[0], screenXY[1], alpha]
      })

      points.sort((a, b) => a.z - b.z)

      points.forEach(point => {
        ctx.beginPath();
        ctx.fillStyle = `rgba(${point.color[0]}, ${point.color[1]}, ${point.color[2]}, ${point.color[3]})`;
        ctx.arc(point.x, point.y, point.radius, 0, 2 * Math.PI);
        ctx.fill();
        ctx.lineWidth = point.radius * 3
        ctx.strokeStyle = `rgba(${point.color[0]}, ${point.color[1]}, ${point.color[2]}, ${point.color[3]*.3})`;
        ctx.stroke();
      })
    }
  }

  // Handle resize events
  #onResize() {
    this.#render();
  }

  // Animation loop
  #animate(timestamp) {
    const deltaTime = timestamp - this.#lastFrameTime;

    // Update the last frame time
    this.#lastFrameTime = timestamp;

    this.#render();
    if (deltaTime > 0) {
      for(let i = 0; i < 3; i++) {
        this.#rotateAngle[i] += this.#rotateSpeed[i] * deltaTime/1000
      }
    }
    requestAnimationFrame((timestamp) => { this.#animate(timestamp) })
  }

  // Clean up observer when element is removed
  disconnectedCallback() {
    this.#resizeObserver.disconnect();
  }

  #vertices = []
  #colorArray = new Uint8Array()

  #radius = 6
  #zoom = 0.4
  #rotateSpeed = [-0.1, 0.15, 0.0]

  #rotateAngle = [0, 0, 0]
  #lastFrameTime = 0

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
customElements.define('led-3d', Led3D);