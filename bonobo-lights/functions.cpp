#include <sys/_stdint.h>
#include "delay.h"
#include "variant.h"
#include <FastLED.h>
#include "config.h"
#include "functions.h"
#include "patterns.h"
#include <Wire.h>

/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////

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

void loadPattern(uint8_t *newPattern, int sizeOfNewPattern) {
  int smallerSize = min(sizeOfNewPattern, sizeof(pattern));
  memcpy(pattern, newPattern, smallerSize);
}

void nextPattern() {
  loadPattern((uint8_t *)all240patterns[currentPattern], angularPixels * NUMPIXELS * 3);

  currentPattern++;
  if (currentPattern == N_PATTERNS) { currentPattern = 0; }
};

void loadSlice(uint8_t *newSlice) {
  for (int i = 0; i < NUMPIXELS; i++) {
    for (int j = 0; j < 3; j++) {
      scratchSlice[i * 3 + j] = newSlice[i * 3 + j] * brightnessCorrection[i];
      Serial.print(scratchSlice[i * 3 + j]);
      Serial.print(", ");
    }
  }
  Serial.println("");
  slice = scratchSlice;
}

// Version for single-color patterns
void loadSlice(bool newSlice[NUMPIXELS], uint32_t color) {
  uint8_t r = (color >> 16);
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;

  // Create color scratch slice
  for (int i = 0; i < NUMPIXELS; i++) {
    if (newSlice[i]) {
      scratchSlice[i * 3] = r;
      scratchSlice[i * 3 + 1] = g;
      scratchSlice[i * 3 + 2] = b;
    } else {
      scratchSlice[i * 3] = 0;
      scratchSlice[i * 3 + 1] = 0;
      scratchSlice[i * 3 + 2] = 0;
    }
  }

  // Change the pointer
  loadSlice((uint8_t *)&scratchSlice);
}

void updateSensor() {
  // Using an analog Hall effect sensor
  previouslyDetected = detected;

  int difference = (int)analogRead(HALL) - 518;  // neutral point
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


// Keeps track of which pixel number are we on and loads the slice (udpates ) if needed
void updatePolarIndex(long now) {
  int previousPolarIndex = polarIndex;
  polarIndex = currentPosition() / angularResolution;



  if (polarIndex != previousPolarIndex) {
    if (polarIndex >= 0 && polarIndex < angularPixels) {
      loadSlice((uint8_t *)&pattern[polarIndex * NUMPIXELS * 3]);
      cleared = false;
    } else if (!cleared) {
      loadSlice((uint8_t *)&blankSlice);
      cleared = true;
    }

    showSlice();
  }
}

// Transmits
void showSlice() {
  for (int i = 0; i < NUMPIXELS; i++) {
    leds[i] = CRGB(slice[3 * i], slice[3 * i + 1], slice[3 * i + 2]);
  }
  FastLED.show();
}


void printSerial() {
  char buffer[150];
  now = micros();
  int polarIndex = currentPosition() / angularResolution;

  if (now - lastSerialPrint > SERIAL_UPDATE) {
    sprintf(buffer, "revolution:%d revolutionStart:%d polarIndex:%d currentDurationOfRevolution:%d\n", revolution, revolutionStart, polarIndex, currentDurationOfRevolution);
    Serial.println(buffer);
    lastSerialPrint = now;
  }
}

void takeUserInput() {
  long now = micros();
  bool prevButton1Pressed = button1Pressed;
  bool prevButton2Pressed = button2Pressed;

  if ((now - lastUIread) > UI_UPDATE) {
    button1Pressed = !digitalRead(BUTTON_1);
    button2Pressed = !digitalRead(BUTTON_2);
    lastUIread = now;
  };

  if (button1Pressed && !prevButton1Pressed) {
    Serial.println("nextPattern");
    nextPattern();
  }
}

void printArray(uint8_t array[], int length) {
  Serial.print("{");
  for (int i = 0; i < length; i++) {
    Serial.print(array[i]);
    if (i < length - 1) {
      Serial.print(", ");
    }
  }
  Serial.println("}");
}

void readI2C(int bytesToRead) {
  int i = 0;
  Serial.println("");
  while (Wire.available() && i < bytesToRead) {
    buffer[i] = Wire.read();  // Read each byte into the buffer
    i++;
  }
  buffer[bytesToRead] = '\0';

  Serial.println(buffer);
}

/////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////

const int allOutputPins[] = { STRIP_DATA, STRIP_CLOCK, LED_R, LED_G };  //  , LED_B };

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
float angularResolution = 1.0 / angularPixels;

bool debug = false;
bool cleared = false;

unsigned long microsecondsPerFrame = 1000000 / frameRate;
unsigned long microsecondsPerRefresh = 1000000 / (refreshRate);

// Framebuffer
// uint8_t slice[NUMPIXELS * 3] = { 0 };

bool primes[32] = { false, true, true, false, true, false, true, false, false, false,
                    true, false, true, false, false, false, true, false, true, false,
                    false, false, true, false, false, false, false, false, true, false,
                    true, false };
const uint8_t blankSlice[NUMPIXELS * 3] = { 0 };
uint8_t *slice = (uint8_t *)&blankSlice;
uint8_t scratchSlice[NUMPIXELS * 3] = { 0 };
uint8_t pattern[angularPixels * NUMPIXELS * 3] = { 0 };
uint8_t currentPattern = 0;
long lastUIread;
bool button1Pressed;
bool button2Pressed;

const float brightnessCorrection[NUMPIXELS] = { 0.2694, 0.2929, 0.3165, 0.3401, 0.3636, 0.3872, 0.4108, 0.4343,
                                                0.4579, 0.4815, 0.5051, 0.5286, 0.5522, 0.5758, 0.5993, 0.6229,
                                                0.6465, 0.6700, 0.6936, 0.7172, 0.7407, 0.7643, 0.7879, 0.8114,
                                                0.8350, 0.8586, 0.8822, 0.9057, 0.9293, 0.9529, 0.9764, 1.0000 };

char buffer[BUFFER_SIZE] = { 0 };