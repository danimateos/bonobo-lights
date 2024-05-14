#include "functions.h"
#include "config.h"

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

void updateSlice(bool newSlice[NUMPIXELS]) {

  if (debug) {
    Serial.print("{");
  }

  for (int i = 0; i < NUMPIXELS; i++) {
    slice[i] = newSlice[i];  // I'm sure there is a better way but I haven't figured it out. memcpy(frame, hey[pixel], nRows * nCols ); and my attempts with pointers crashed the board.
    if (debug) {
      Serial.print(slice[i]);
      Serial.print(", ");
    }
  }

  if (debug) { Serial.println("}"); }
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

  if (polarIndex != previousPolarIndex) {
    if (polarIndex >= 0 && polarIndex < sizeOfPattern) {
      updateSlice(pattern[polarIndex]);
    } else {
      updateSlice(blank);
    }
  }
}

void showSlice(uint32_t color) {

  for (int i = 0; i < NUMPIXELS; i++) {
    if (slice[i]) {
      leds[i] = color;
    } else {
      leds[i] = CRGB::Black;
    }
  }
  FastLED.show();
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
float angularResolution = 1.0 / 120;

bool debug = false;


unsigned long microsecondsPerFrame = 1000000 / frameRate;
unsigned long microsecondsPerRefresh = 1000000 / (refreshRate);

// Framebuffer
bool primes[NUMPIXELS] = { false, true, true, false, true, false, true, false, false, false, true, false, true, false, false, false };
bool blank[NUMPIXELS] = { false };
bool slice[NUMPIXELS] = { false };
bool pattern[sizeOfPattern][NUMPIXELS] = { { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1 },
                                           { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },
                                           { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },
                                           { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },
                                           { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1 },
                                           { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
                                           { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1 },
                                           { 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
                                           { 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
                                           { 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 },
                                           { 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 },
                                           { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
                                           { 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
                                           { 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1 },
                                           { 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1 },
                                           { 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1 },
                                           { 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
                                           { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
                                           { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1 } };

