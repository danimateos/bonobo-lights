#include <FastLED.h>
#include <Wire.h>

#include "config.h"
#include "functions.h"
#include "patterns.h"

CRGB leds[NUMPIXELS];

void setup() {

  pinMode(STRIP_DATA, OUTPUT);
  pinMode(STRIP_CLOCK, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);

  pinMode(HALL, INPUT);
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);

  FastLED.addLeds<DOTSTAR, STRIP_DATA, STRIP_CLOCK, BGR, DATA_RATE_MHZ(24)>(leds, NUMPIXELS);
  FastLED.setBrightness(100);

  Wire.begin(ADDRESS);
  Wire.onReceive(readI2C);

  Serial.begin(115200);
  sliceOnMicros = micros();



  loadPattern((uint8_t *)coolData, angularPixels * NUMPIXELS * 3);
}

void loop() {
  now = micros();

  updateSensor();  // Every single turn of the loop we check the sensor and update our speed estimation

  updatePolarIndex(now);  // calculates which pixel column and updates the frame. Uncomment for dynamic display
  // loadSlice(primes, CRGB::Crimson);  // Fixed test pattern. Comment to get dynamic display.
  showSlice();

  takeUserInput();
  printSerial();

  step += 1;
}
