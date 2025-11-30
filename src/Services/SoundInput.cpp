#include "SoundInput.h"
#include <i2s.h>

#define SIGNAL_FREQUENCY 11025
#define SAMPLES 128                // Must be a power of 2
#define SAMPLING_FREQUENCY 2000   // Sampling frequency in Hz

float vReal[SAMPLES];
float vImag[SAMPLES];

ArduinoFFT<float> FFT = ArduinoFFT<float>(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY);

namespace Services {
  SoundInput *SoundInput::instance = nullptr;

  SoundInput::SoundInput(FS& fs): fs(fs) {
  }

  float* SoundInput::getVolumeByFrequency() {
    return vReal;
  }

  void SoundInput::setup() {
    i2s_rxtx_begin(true, false); // Enable I2S RX
    i2s_set_rate(SIGNAL_FREQUENCY);
  }

  void SoundInput::loop() {
    readI2S();
    computeFFT();

    /*
    Serial.println("EQ");
    float frequencyResolution = (float)SAMPLING_FREQUENCY / SAMPLES;
    for (int i = 0; i < (SAMPLES / 2); i++) {
      double amplitude = vReal[i];
      int bars = (int)(amplitude / 100.0);
      if (bars > 50) bars = 50;  // Limit the max number of asterisks

      Serial.print(i * frequencyResolution);
      Serial.print("Hz: ");
      for (int j = 0; j < bars; j++) {
        Serial.print("*");
      }
      Serial.println();
    }
    */
  }

  void SoundInput::readI2S() {
    int16_t left[SAMPLES];
    int16_t right;
    for(int i = 0; i < SAMPLES; i++) {
      i2s_read_sample(left + i, &right, true);
    }

    for (int i = 0; i < SAMPLES; i++) {
      vReal[i] = (float)left[i];
      vImag[i] = 0.0;
    }
  }

  void SoundInput::computeFFT() {
    FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);	/* Weigh data */
    FFT.compute(FFTDirection::Forward); /* Compute FFT */
    FFT.complexToMagnitude(); /* Compute magnitudes */
    float x = FFT.majorPeak();
  }
}