import fs from 'fs'
import htmlMinifier from 'html-minifier'
import CleanCSS from 'clean-css'
import { minify as Terser } from 'terser'
import zlib from 'zlib'

// Source folder, where the original files are stored
const assetsFolder = 'assets'
// Output, where the compressed header files are created
const contentFolder = 'src/Web/UI/Content'

console.info('Bundling web assets...')

if (!fs.existsSync(contentFolder)){
  fs.mkdirSync(contentFolder);
}

bundleHtml(`${assetsFolder}/html/index.html`, `${contentFolder}/index_html.h`, 'index_html')
bundleCss(`${assetsFolder}/css/app.css`, `${contentFolder}/app_css.h`, 'app_css')
await bundleJs(`${assetsFolder}/js/app.js`, `${contentFolder}/app_js.h`, 'app_js')

process.exit(0)

// Minify, compress, and generate a header file with a PROGMEM block of the HTML file
function bundleHtml(source, destination, variableName) {
  console.info(`Processing file ${source}`)
  const content = fs.readFileSync(source).toString()
  const originalSize = content.length

  console.info(' - Minifying HTML')
  const minifyOptions = {
    includeAutoGeneratedTags: true,
    removeAttributeQuotes: true,
    removeComments: true,
    removeRedundantAttributes: true,
    removeScriptTypeAttributes: true,
    removeStyleLinkTypeAttributes: true,
    sortClassName: true,
    useShortDoctype: true,
    collapseWhitespace: true
  }
  const output = htmlMinifier.minify(content, minifyOptions)
  console.info(` - Resized ${resizeText(originalSize, output.length)}`)

  bundleGzippedArray(source, output, destination, variableName)
}

// Minify, compress, and generate a header file with a PROGMEM block of the CSS file
function bundleCss(source, destination, variableName) {
  console.info(`Processing file ${source}`)
  const content = fs.readFileSync(source)

  console.info(' - Minifying CSS')
  const minifyOptions = {}
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

// Minify, compress, and generate a header file with a PROGMEM block of the JS file
async function bundleJs(source, destination, variableName) {
  console.info(`Processing ${source}`)
  const content = fs.readFileSync(source).toString()
  const originalSize = content.length

  console.info(' - Minifying JS')
  const minifyOptions = {}
  const result = await Terser(content, minifyOptions)
  console.info(` - Resized ${resizeText(originalSize, result.code.length)}`)
  bundleGzippedArray(source, result.code, destination, variableName)
}

// Generate a header file with the comment preable and a char[] PROGMEM in plaintext
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

// Generate a header file with the comment premble and hexidecimal binary array
function bundleGzippedArray(source, data, destination, variableName) {
  const originalSize = data.length
  const zipOptions = { 
    level: zlib.constants.Z_BEST_COMPRESSION 
  }
  try {
    const result = zlib.gzipSync(data, zipOptions)
    console.info(` - Compressed ${resizeText(originalSize, result.length)}`)
    data = toHexBlock(result)

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

// Write the file
function writeFile(content, destination) {
  console.info(` - Writing ${destination}`)
  fs.writeFileSync(destination, content)
}

// Some useful text showing how much a file was resized
function resizeText(from, to) {
  return `from ${from.toLocaleString()} to ${to.toLocaleString()} bytes (${Math.round(1000*(from-to)/from/10)}%)`
}

// Convert the data into lines of hexidecimal codes
function toHexBlock(data) {
  const lineLength = 32
  let lines = []
  for (let i = 0; i < data.length; i += lineLength) {
    let hexArray = []
    
    const block = data.slice(i, i + lineLength)
    for (let value of block) {
      hexArray.push('0x' + value.toString(16).padStart(2, '0'))
    }

    const hexString = hexArray.join(', ')
    lines.push(`  ${hexString}`)
  }

  return lines.join(',\n')
}