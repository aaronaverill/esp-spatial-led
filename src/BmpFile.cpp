#include "BmpFile.h"
#include "Services/LedDriver.h"

void BmpFile::writeLedData(AsyncWebServerRequest *request) {
    Services::LedDriver& leds = Services::LedDriver::getInstance();

    size_t ledCount = leds.getRenderLedCount();
    size_t rgbSize = ledCount * 3;
    size_t rowSize = (rgbSize + 3) & ~3; // Rows are padded up to the nearest 4 byte boundary
    size_t bmpHeaderSize = 14 + 40; // Bitmap header size + DIB BITMAPINFOHEADER size
    size_t bmpSize = bmpHeaderSize + rowSize;

    // Compiler gives unused-but-set-variable warning even though we are using this in the closure below
    uint8_t __attribute__((unused)) bmpHeader[] = {
      // Bitmap header
      0x42, 0x4D, // "B", "M"
      (uint8_t)(bmpSize & 0xFF), (uint8_t)((bmpSize >> 8) & 0xFF), (uint8_t)((bmpSize >> 16) & 0xFF), (uint8_t)((bmpSize >> 24) & 0xFF), // Size of bitmap file, little endian
      0x00, 0x00, 0x00, 0x00, // Reserved bytes
      (uint8_t)bmpHeaderSize, 0x00, 0x00, 0x00, // Start of bitmap data, after headers
      // DIB BITMAPINFOHEADER
      40, 0x00, 0x00, 0x00, // Size of header
      (uint8_t)(ledCount & 0xFF), (uint8_t)((ledCount >> 8) & 0xFF), (uint8_t)((ledCount >> 16) & 0xFF), (uint8_t)((ledCount >> 24) & 0xFF), // Width in pixels
      0x01, 0x00, 0x00, 0x00, // Height in pixels
      0x01, 0x00, // Number of color planes
      24, 0x00, // Bits per pixel
      0x00, 0x00, 0x00, 0x00, // Compression: None
      (uint8_t)(rowSize & 0xFF), (uint8_t)((rowSize >> 8) & 0xFF), (uint8_t)((rowSize >> 16) & 0xFF), (uint8_t)((rowSize >> 24) & 0xFF), // Width in pixels
      0x13, 0x0B, 0x00, 0x00, // Horizontal resolution: 72dpi
      0x13, 0x0B, 0x00, 0x00, // Vertictal resolution: 72dpi
      0x00, 0x00, 0x00, 0x00, // Colors per palette
      0x00, 0x00, 0x00, 0x00 // Number of important colors
    };

    const size_t size = bmpSize;

    AsyncWebServerResponse *response = request->beginResponse("image/bmp", size, 
      [bmpHeader, bmpHeaderSize, size] (uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
      //Write up to "maxLen" bytes into "buffer" and return the amount written.
      //index equals the amount of bytes that have been already sent
      //You will not be asked for more bytes once the content length has been reached.
      //Keep in mind that you can not delay or yield waiting for more data!
      //Send what you currently have and you will be asked for more again
      Services::LedDriver& leds = Services::LedDriver::getInstance();
      uint8_t* rgbData = (uint8_t*)leds.getLeds();
      size_t ledCount = leds.getRenderLedCount();
      size_t rgbSize = ledCount * 3;

      size_t bytesSent = 0;
      while (index < size && bytesSent < maxLen) {
        size_t chunkSize;
        if (index < bmpHeaderSize) {
          size_t headerRemaining = bmpHeaderSize - index;
          chunkSize = min(maxLen - bytesSent, headerRemaining);
          memcpy(buffer + bytesSent, bmpHeader+index, chunkSize);

        } else if (index < (bmpHeaderSize + rgbSize)) {
          size_t rgbRemaining = bmpHeaderSize + rgbSize - index;
          chunkSize = min(maxLen - bytesSent, rgbRemaining);
          memcpy(buffer + bytesSent, rgbData + index - bmpHeaderSize, chunkSize);

        } else {
          chunkSize = maxLen - bytesSent;
          memset(buffer + bytesSent, 0, chunkSize);

        }
        index += chunkSize;
        bytesSent += chunkSize;
      }
      return bytesSent;
    });

    request->send(response);
}