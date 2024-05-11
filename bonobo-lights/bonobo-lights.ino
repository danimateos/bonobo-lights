#include <Adafruit_DotStar.h>
#include <SPI.h>

#include "config.h"
#include "functions.h"

Adafruit_DotStar strip(NUMPIXELS, STRIP_DATA, STRIP_CLOCK, DOTSTAR_BGR);




void setup() {

  for (int i = 0; i < 8; i++) {
    // pinsOut |= (1 << allOutputPins[i]);
    pinMode(allOutputPins[i], OUTPUT);
  }
  // PORT_IOBUS->Group[0].OUTSET.reg = pinsOut; // not working yet

  pinMode(HALL, INPUT);

  strip.begin();       // Initialize pins for output
  strip.updatePins();  // Switch over to hardware SPI
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

  showSlice(0xFFAA00);

  step += 1;
}

void updateSensor() {
  // Using an analog Hall effect sensor
  previouslyDetected = detected;

  int difference = (int)analogRead(HALL) - 520;  // neutral point
  detected = abs(difference) > HALL_MARGIN;

  if (debug) {
    Serial.print("analog:");
    Serial.print(analogRead(HALL));
    Serial.print(" diff:");
    Serial.print(difference);
    Serial.print(" detected:");
    Serial.println(detected);
  }

  if (detected && !previouslyDetected) {
    updateSpeedEstimation();
    revolution += 1;
  }
}

// For now, just use the last turn. Later we can go to the exponential smoothing version
void updateSpeedEstimation() {
  long now = micros();
  currentDurationOfRevolution = now - revolutionStart;
  revolutionStart = micros();
}

// Fractions of a revolution
float currentPosition() {
  double elapsed = micros() - revolutionStart;
  return elapsed / currentDurationOfRevolution;
}




// Keeps track of which pixel number are we on and updates the frame if needed
void updatePolarIndex(long now) {
  int previousPolarIndex = polarIndex;
  polarIndex = (currentPosition() - offset) / angularResolution;

  if (polarIndex != previousPolarIndex && polarIndex >= 0 && polarIndex < sizeOfPattern) {
    updateSlice(pattern[polarIndex]);
  }
}

void showSlice(uint32_t color) {

  for (int i = 0; i < NUMPIXELS; i++) {
    if (slice[i]) {
      strip.setPixelColor(i, color);
    } else {
      strip.setPixelColor(i, 0);
    }
  }
  strip.show();
}


void printSerial() {
  char buffer[150];
  now = micros();

  if (now - lastSerialPrint > SERIAL_UPDATE) {
    sprintf(buffer, "revolution:%d revolutionStart:%d current_postion:%.3f currentDurationOfRevolution:%d\n", revolution, revolutionStart, currentPosition(), currentDurationOfRevolution);
    Serial.println(buffer);
    lastSerialPrint = now;
  }
}