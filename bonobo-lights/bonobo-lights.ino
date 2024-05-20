#include <FastLED.h>
#include <SPI.h>

#include "config.h"
#include "functions.h"
#include "patterns.h"

CRGB leds[NUMPIXELS];

void setup() {

  for (int i = 0; i < 8; i++) {
    // pinsOut |= (1 << allOutputPins[i]);
    pinMode(allOutputPins[i], OUTPUT);
  }
  // PORT_IOBUS->Group[0].OUTSET.reg = pinsOut; // not working yet

  pinMode(HALL, INPUT);

  FastLED.addLeds<DOTSTAR, STRIP_DATA, STRIP_CLOCK, BGR, DATA_RATE_MHZ(24)>(leds, NUMPIXELS);
  FastLED.setBrightness(25);

  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, HIGH);

  Serial.begin(115200);
  sliceOnMicros = micros();

  // loadSlice(primes, CRGB::Crimson);
  for (int i = 0; i < NUMPIXELS; i++) {
    all[i] = true;
  };

  int smallerSize = min(sizeof(cool), sizeof(pattern));
  memcpy(pattern, cool, smallerSize);
}

void loop() {
  now = micros();

  updateSensor();  // Every single turn of the loop we check the sensor and update our speed estimation

  updatePolarIndex(now);  // calculates which pixel column and updates the frame. Uncomment for dynamic display

  // loadSlice(all, 0x777777);  // Fixed test pattern. Comment to get dynamic display.
  showSlice();

  printSerial();

  step += 1;
}
