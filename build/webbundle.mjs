import fs from 'fs'
import CleanCSS from 'clean-css'
import { minify } from 'terser'
import zlib from 'zlib'

const assetsFolder = 'assets'
const contentFolder = 'src/Web/UI/Content'

console.info('Bundling web assets...')

if (!fs.existsSync(contentFolder)){
  fs.mkdirSync(contentFolder);
}

bundleHtml(`${assetsFolder}/html/index.html`, `${contentFolder}/index_html.h`, 'index_html')
bundleCss(`${assetsFolder}/css/app.css`, `${contentFolder}/app_css.h`, 'app_css')
await bundleJs(`${assetsFolder}/js/app.js`, `${contentFolder}/app_js.h`, 'app_js')

function bundleHtml(source, destination, variableName) {
  console.info(`Processing file ${source}`)
  const content = fs.readFileSync(source)
  bundleGzippedArray(source, content, destination, variableName)
}

function bundleCss(source, destination, variableName) {
  console.info(`Processing file ${source}`)
  const content = fs.readFileSync(source)

  console.info(' - Minifying CSS')
  const minifyOptions = {
    /* options */ 
  }
  const output = new CleanCSS(minifyOptions).minify(content)
  //console.info(output)
  if (output.errors.length) {
    console.info(' - Errors:')
    for(let i = 0; i < output.errors.length; i++) {
      console.info('    ' + output.errors[i])
    }
    process.exitCode = 1
    process.exit()
  }
  console.info(` - Resized ${resizeText(output.stats.originalSize, output.stats.minifiedSize)}`)
  if (output.warnings.length) {
    console.info(' - Warnings:')
    for(let i = 0; i < output.warnings.length; i++) {
      console.info('    ' + output.warnings[i])
    }
  }

  bundleGzippedArray(source, output.styles, destination, variableName)
}

async function bundleJs(source, destination, variableName) {
  console.info(`Processing ${source}`)
  const content = fs.readFileSync(source).toString()
  const originalSize = content.length

  console.info(' - Minifying JS')
  const minifyOptions = {}
  const result = await minify(content, minifyOptions)
  console.info(` - Resized ${resizeText(originalSize, result.code.length)}`)
  bundleGzippedArray(source, result.code, destination, variableName)
}

function bundleRawString(source, data, destination, variableName) {
  const content = `
// File autogenerated from ${source} by build/webbundle.js. 
// Do not edit.

#include <Arduino.h>

const char ${variableName}[] PROGMEM = R"rawstring(
${data}
)rawstring";
`
  writeFile(content, destination)
}

function bundleGzippedArray(source, data, destination, variableName) {
  const originalSize = data.length
  const zipOptions = { 
    level: zlib.constants.Z_BEST_COMPRESSION 
  }
  try {
    const result = zlib.gzipSync(data, zipOptions)
    console.info(` - Compressed ${resizeText(originalSize, result.length)}`)
    data = hexdump(result)

    const content = `
// File autogenerated from ${source} by build/webbundle.js. 
// Do not edit.

#include <Arduino.h>

const uint16_t ${variableName}_length = ${result.length};
const uint8_t ${variableName}[] PROGMEM = {
${data}
};
`
    writeFile(content, destination)

  } catch (ex) {
    console.info(' - GZip Error:')
    console.info(ex)
    process.exitCode = 1
    process.exit()
  }
}

function writeFile(content, destination) {
  console.info(` - Writing ${destination}`)
  fs.writeFileSync(destination, content)
}

function resizeText(from, to) {
  return `from ${from.toLocaleString()} to ${to.toLocaleString()} bytes (${Math.round(1000*(from-to)/from/10)}%)`
}

function hexdump(buffer, isHex = false) {
  let lines = []

  for (let i = 0; i < buffer.length; i +=(isHex?32:16)) {
    var block
    let hexArray = []
    if (isHex) {
      block = buffer.slice(i, i + 32)
      for (let j = 0; j < block.length; j +=2 ) {
        hexArray.push('0x' + block.slice(j,j+2))
      }
    } else {
      // cut buffer into blocks of 16
      block = buffer.slice(i, i + 16)
      for (let value of block) {
        hexArray.push('0x' + value.toString(16).padStart(2, '0'))
      }
    }

    let hexString = hexArray.join(', ')
    let line = `  ${hexString}`
    lines.push(line)
  }

  return lines.join(',\n')
}