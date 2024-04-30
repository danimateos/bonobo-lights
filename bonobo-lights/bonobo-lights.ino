
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

// Settings
const int SERIAL_UPDATE = 100000;  // print every SERIAL_UPDATE microseconds
const int frameRate = 12;          // For video
unsigned long microsecondsPerFrame = 1000000 / frameRate;
const unsigned int refreshRate = 5000;
unsigned long microsecondsPerRefresh = 1000000 / refreshRate;
const float angularResolution = 1.0 / 120;  // Angular width in fractions of a revolution of a pixel column


// Holding the patterns as arrays of ints
bool primes[nRows * nCols] = { false, true, true, false, true, false, true, false, false, false, true, false, true, false, false, false };
bool blank[nRows * nCols] = { false };
bool *frame = primes;
const int sizeOfHey = 19;
bool hey[sizeOfHey][nRows * nCols] = { { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1 },
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
long currentDurationOfRevolution = 100000000;
int pixel = 0;
float offset = .1;
long frameNumber = 0;
long revolution = 0;
byte prevRow = 0;
byte currentRow = 0;
bool pressed = false;
bool previouslyPressed = false;
long lastSerialPrint;
long step = 0;
long rowStart, now, frameStart, revolutionStart;

// For performance profiling
long busyMicros = 0;
bool performanceProfiling = false;
bool debug = false;

void setup() {

  // long pinsOut = 0;
  for (int i = 0; i < 8; i++) {
    // pinsOut |= (1 << allOutputPins[i]);
    pinMode(allOutputPins[i], OUTPUT);
  }
  // PORT_IOBUS->Group[0].OUTSET.reg = pinsOut; // not working yet

  pinMode(HALL_SENSOR_PIN_DIGITAL, INPUT);

  Serial.begin(115200);
  delay(500);
  Serial.println("Let us play");
  rowStart = micros();
  frameStart = rowStart;
}

void loop() {
  now = micros();

  updateSensor();  // Every single turn of the loop we check the sensor and update our speed estimation

  prevRow = currentRow;
  updateRowState(now);    // updates currentRow for muxing
  updatePixelState(now);  // calls updateFrame() on a fixed schedule
  if (currentRow != prevRow) {
    rowShow(currentRow, frame);  // switch on and off the appropriate LEDs
  }

  printSerial();  // On a fixed schedule

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

// Fractions of a revolution
float currentPosition() {
  double elapsed = micros() - revolutionStart;
  return elapsed / currentDurationOfRevolution;
}

// Keeps track of which pixel number are we on
void updatePixelState(long now) {
  pixel = (currentPosition() - offset) / angularResolution;

  if (pixel < sizeOfHey) {
    frame = hey[pixel];
  } else {
    frame = blank;
  }

  Serial.println(pixel);
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