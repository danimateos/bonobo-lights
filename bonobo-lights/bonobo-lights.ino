#include <Adafruit_DotStar.h>
#include <SPI.h>

#include "config.h"
#include "functions.h"


void setup() {

  for (int i = 0; i < 8; i++) {
    // pinsOut |= (1 << allOutputPins[i]);
    pinMode(allOutputPins[i], OUTPUT);
  }
  // PORT_IOBUS->Group[0].OUTSET.reg = pinsOut; // not working yet

  pinMode(HALL, INPUT);

  strip.begin();       // Initialize pins for output
  // strip.updatePins();  // Switch over to hardware SPI
  strip.show();        // Turn all LEDs off ASAP

  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, HIGH);

  Serial.begin(115200);
  delay(100);
  Serial.println("Let us play");
  sliceOnMicros = micros();

  updateSlice(primes);
}

void loop() {
  now = micros();

  updateSensor();  // Every single turn of the loop we check the sensor and update our speed estimation

  updatePolarIndex(now);  // calculates which pixel column and updates the frame

  showSlice(0xFF4400);

  step += 1;
}
