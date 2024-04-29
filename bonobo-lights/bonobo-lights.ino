
const int r0 = 0;
const int r1 = 1;
const int r2 = 2;
const int r3 = 3;

const int c0 = 4;
const int c1 = 5;
const int c2 = 6;
const int c3 = 7;

const int nRows = 4;
const int nCols = 4;

int rows[nRows] = { r0, r1, r2, r3 };
int cols[nCols] = { c0, c1, c2, c3 };
int allOutputPins[nRows + nCols] = { r0, r1, r2, r3, c0, c1, c2, c3 };

const int HALL_SENSOR_PIN_DIGITAL = 10;  // board_02

const int SERIAL_UPDATE = 100000;  // microseconds
const unsigned int frameRate = 12;
unsigned long microsecondsPerFrame = 1000000 / frameRate;
const unsigned int refreshRate = 5000;
unsigned long microsecondsPerRefresh = 1000000 / (refreshRate);

bool primes[nRows * nCols] = { false, true, true, false, true, false, true, false, false, false, true, false, true, false, false, false };
bool frame[nRows * nCols] = { false };

long step = 0;
long rowStart, now, frameStart, revolutionStart;

// For position estimation with exponential smoothing. In microseconds.
// Initialize durations to very high values (10^8us) so that there's
// no graphical updates until after 5 revolutions.
const int nRevolutionsToConsider = 5;
const int weights[nRevolutionsToConsider] = { 315, 625, 1250, 2500, 5000 };  // most recent weighs heavier
const int sumOfWeights = 9690;
long revolutionDurations[nRevolutionsToConsider] = { 100000000, 100000000, 100000000, 100000000, 100000000 };
long currentDurationOfRevolution = 100000000;

// State machine. A frame is a superstate of which each row represents
// one of 4 distinct substates, but I'm managing them independently
long frameNumber = 0;
long revolution = 0;
byte prevRow = 0;
byte currentRow = 0;
bool pressed = false;
bool previouslyPressed = false;
long lastSerialPrint;


// For performance profiling
long busyMicros = 0;
bool performanceProfiling = true;
bool debug = false;

void setup() {

  // long pinsOut = 0;
  for (int i = 0; i < 8; i++) {
    // pinsOut |= (1 << allOutputPins[i]);
    pinMode(allOutputPins[i], OUTPUT);
  }
  // PORT_IOBUS->Group[0].OUTSET.reg = pinsOut; // not working yet

  Serial.begin(115200);
  delay(100);
  Serial.println("Let us play");
  rowStart = micros();
  frameStart = rowStart;
}

void loop() {
  now = micros();

  prevRow = currentRow;
  updateRowState(now);
  updateFrameState(now);

  if (currentRow != prevRow) {
    rowShow(currentRow, frame);
  }

  updateSensor();

  step += 1;
}

void updateSensor() {
  previouslyPressed = pressed;

  pressed = !digitalRead(HALL_SENSOR_PIN_DIGITAL);  // Sensor is pulled up

  if (pressed && !previouslyPressed) {
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

// Exponential smoothing for smoother estimation
// To be debugged
void exponentialSmoothing() {
  long now = micros();
  int offset = revolution % nRevolutionsToConsider;  // circular buffer
  revolutionDurations[offset] = now - revolutionStart;
  revolutionStart = now;

  long tempEstimation = 0;

  for (int i = 0; i < nRevolutionsToConsider; i++) {

    int whichWeight = (i + ((nRevolutionsToConsider - 1) - offset)) % nRevolutionsToConsider;  // Black magic. Notes on Remarkable

    tempEstimation += weights[whichWeight] * revolutionDurations[i];
  }

  currentDurationOfRevolution = tempEstimation / sumOfWeights;
}


// Fractions of a revolution
float currentPosition() {
  double elapsed = micros() - revolutionStart;
  return elapsed / currentDurationOfRevolution;
}

// Keeps track of which frame should be visible now, for movement
void updateFrameState(long now) {
  long elapsed = now - frameStart;

  if (elapsed >= microsecondsPerFrame) {
    updateFrame();
    frameStart = micros();
    frameNumber += 1;
  }
}

// Keeps track of which row should be active now, for muxing
void updateRowState(long now) {
  long elapsed = now - rowStart;
  int segment = elapsed / (microsecondsPerRefresh / nRows);

  if (segment > nRows - 1) {
    currentRow = 0;

    if (performanceProfiling) {
      Serial.print(" busyMicros: ");
      Serial.print(busyMicros);
      Serial.print(" elapsed:");
      Serial.println(elapsed);
    }

    busyMicros = 0;
    rowStart = micros();
  } else {
    currentRow = segment;
  }
}

// Changes the frame in-place. Determines the animation.
// Choose between fallingPrimes, staticPrimes, cylonEyes.
void updateFrame() {
  fallingPrimes();
}

void fallingPrimes() {
  memset(frame, 0, sizeof(frame));

  int offset = frameNumber % sizeof(frame);

  for (int i = 0; i < sizeof(frame); i++) {
    frame[i] = primes[(i - offset) % sizeof(frame)];
  }
}

void staticPrimes() {
  for (int i = 0; i < sizeof(primes); i++) {
    frame[i] = primes[i];
  }
}

void cylonEyes() {
  memset(frame, 0, sizeof(frame));

  int position = frameNumber % (2 * sizeof(frame));  // 0 to 31 if sizeofFrame is 16
  if (position >= sizeof(frame)) {
    int fromEnd = sizeof(frame) - position;
    position = sizeof(frame) + fromEnd;
  }

  frame[position] = true;
}

void rowShow(int rowNumber, bool pattern[]) {
  long thisStart = micros();

  allOff();
  fastWrite(rows[rowNumber], HIGH);

  for (int i = 0; i < nCols; i++) {
    int position = rowNumber * nRows + i;
    if (pattern[position]) {
      fastWrite(cols[i], HIGH);
    }
  }

  busyMicros += micros() - thisStart;
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