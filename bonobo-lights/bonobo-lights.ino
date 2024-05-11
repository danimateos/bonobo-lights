#include <Adafruit_DotStar.h>
#include <SPI.h>

#define NUMPIXELS 16  // Number of LEDs in strip
#define STRIP_DATA 15
#define STRIP_CLOCK 14
Adafruit_DotStar strip(NUMPIXELS, STRIP_DATA, STRIP_CLOCK, DOTSTAR_BGR);

// Status indicator LED
#define LED_R 0
#define LED_G 1
#define LED_B 2

// Human input
#define BUTTON_1 3
#define BUTTON_2 6

// Sensor
#define HALL 7

const int allOutputPins[] = { STRIP_DATA, STRIP_CLOCK, LED_R, LED_G, LED_B };

// UI Settings
#define SERIAL_UPDATE 100000        // print every SERIAL_UPDATE microseconds
const unsigned int frameRate = 12;  // For video
unsigned long microsecondsPerFrame = 1000000 / frameRate;
const unsigned int refreshRate = 5000;
unsigned long microsecondsPerRefresh = 1000000 / (refreshRate);
#define HALL_MARGIN 40  // Lower for more sensitivity, higher for more accuracy. Between 0 and 500.

// Framebuffer
bool primes[NUMPIXELS] = { false, true, true, false, true, false, true, false, false, false, true, false, true, false, false, false };
bool blank[NUMPIXELS] = { false };
bool slice[NUMPIXELS] = { false };
const int sizeOfPattern = 19;
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

//  From https://forum.arduino.cc/t/what-is-the-fastest-way-to-read-write-gpios-on-samd21-boards/907133/9
static inline void fastWrite(int bitnum, int val) {
  if (val)
    PORT_IOBUS->Group[0].OUTSET.reg = (1 << bitnum);
  else
    PORT_IOBUS->Group[0].OUTCLR.reg = (1 << bitnum);
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