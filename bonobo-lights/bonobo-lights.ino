#include <FastLED.h>
#include <SPI.h>

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

  FastLED.addLeds<DOTSTAR, STRIP_DATA, STRIP_CLOCK, BGR, DATA_RATE_MHZ(24)>(leds, NUMPIXELS);
  FastLED.setBrightness(25);

  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);

  Serial.begin(115200);
  sliceOnMicros = micros();


  int smallerSize = min(sizeof(coolData), sizeof(pattern));
  memcpy(pattern, coolData, smallerSize);

}

void loop() {
  now = micros();

  updateSensor();  // Every single turn of the loop we check the sensor and update our speed estimation

  updatePolarIndex(now);  // calculates which pixel column and updates the frame. Uncomment for dynamic display

  // loadSlice(primes, CRGB::Crimson);  // Fixed test pattern. Comment to get dynamic display.
  showSlice();
  printSerial();

  step += 1;
}
