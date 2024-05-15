#include <sys/_stdint.h>
#include <FastLED.h>
#include "config.h"
#include "functions.h"
#include "patterns.h"

//  From https://forum.arduino.cc/t/what-is-the-fastest-way-to-read-write-gpios-on-samd21-boards/907133/9
static inline void fastWrite(int bitnum, int val) {
  if (val)
    PORT_IOBUS->Group[0].OUTSET.reg = (1 << bitnum);
  else
    PORT_IOBUS->Group[0].OUTCLR.reg = (1 << bitnum);
}

void allOff() {
  for (int i = 0; i < 8; i++) {
    fastWrite(allOutputPins[i], LOW);
  }
}

void allOn() {
  for (int i = 0; i < 8; i++) {
    fastWrite(allOutputPins[i], HIGH);
  }
}

void loadSlice(uint8_t newSlice[NUMPIXELS * 3]) {
  for (int i = 0; i < NUMPIXELS * 3; i++) {
    memcpy(slice, newSlice, NUMPIXELS * 3);  // I'm sure there is a better way but I haven't figured it out. memcpy(frame, hey[pixel], nRows * nCols ); and my attempts with pointers crashed the board.
  }
}

void loadSlice(bool newSlice[NUMPIXELS], uint32_t color) {
  uint8_t r = (color >> 16);
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;

  for (int i = 0; i < NUMPIXELS; i++) {
    if (newSlice[i]) {
      slice[i * 3] = r;
      slice[i * 3 + 1] = g;
      slice[i * 3 + 2] = b;  // I'm sure there is a better way but I haven't figured it out. memcpy(frame, hey[pixel], nRows * nCols ); and my attempts with pointers crashed the board.}
    } else {
      slice[i * 3] = 0;
      slice[i * 3 + 1] = 0;
      slice[i * 3 + 2] = 0;
    }
  }
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


// Keeps track of which pixel number are we on and loads the slice if needed
void updatePolarIndex(long now) {
  int previousPolarIndex = polarIndex;
  polarIndex = (currentPosition() - offset) / angularResolution;

  if (polarIndex != previousPolarIndex) {
    if (polarIndex >= 0 && polarIndex < angularPixels) {
      loadSlice(&pattern[polarIndex * NUMPIXELS * 3]);
    } else {
      loadSlice(blankSlice);
    }
  }
}

void showSlice() {
  for (int i = 0; i < NUMPIXELS; i++) {
    leds[i] = CRGB(slice[3 * i], slice[3 * i + 1], slice[3 * i + 2]);
  }
  FastLED.show();
}

void loadPrimes(uint32_t color) {
  // TODO use the primes bool array
  for (int i = 0; i < NUMPIXELS; i++) {
    if (primes[i]) {
      slice[i] = color >> 16;     // R
      slice[i + 1] = color >> 8;  // G
      slice[i + 2] = color >> 0;  // B
    } else {
      slice[i] = 0;      // R
      slice[i + 1] = 0;  // G
      slice[i + 2] = 0;  // B
    }
  }
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

// Variables

const int allOutputPins[] = { STRIP_DATA, STRIP_CLOCK, LED_R, LED_G, LED_B };

// State machine. A frame is a superstate of which each row represents
// one of 4 distinct substates, but I'm managing them independently
long revolution = 0;
bool detected = false;
bool previouslyDetected = false;
long lastSerialPrint;
long step = 0;
long now, sliceOnMicros, revolutionStart;
long currentDurationOfRevolution = 100000000;
int polarIndex = 0;
float offset = .1;
float angularResolution = 1.0 / angularPixels;

bool debug = false;


unsigned long microsecondsPerFrame = 1000000 / frameRate;
unsigned long microsecondsPerRefresh = 1000000 / (refreshRate);

// Framebuffer
// uint8_t slice[NUMPIXELS * 3] = { 0 };

bool primes[NUMPIXELS] = { false, true, true, false, true, false, true, false, false, false, true, false, true, false, false, false };
uint8_t blankSlice[NUMPIXELS * 3] = { 0 };
uint8_t slice[NUMPIXELS * 3] = { 0 };
uint8_t pattern[angularPixels * NUMPIXELS * 3] = { 0 };
// memcpy(pattern, dudemabaike, sizeof(dudemabaike));
// uint8_t pattern[angularPixels * NUMPIXELS * 3] = memcpy(void *, const void *, size_t);
