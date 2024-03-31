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
int all_pins[nRows + nCols] = { r0, r1, r2, r3, c0, c1, c2, c3 };

const unsigned int frameRate = 20;
unsigned long microsecondsPerFrame = 1000000 / frameRate;
const unsigned int refreshRate = 1000;
unsigned long microsecondsPerRefresh = 1000000 / (refreshRate);

// For performance profiling
long busyMicros = 0;
float busyFraction = 0.0;

bool pattern[nRows * nCols];
bool primes[nRows * nCols] = { false, true, true, false, true, false, true, false, false, false, true, false, true, false, false, false };
bool frame[nRows * nCols] = { false };

long iteration = 0;
long rowStart, now, frameStart;
int nCyclesRefresh = 10000;

// State machine. A frame is a superstate of which each row
// represents one of 4 distinct substates, but I'm managing them independently
long frameNumber = 0;
byte prevRow = 0;
byte currentRow = 0;

bool debug = false;
bool performanceProfiling = false;

void setup() {

  for (int i = 0; i < 8; i++) {
    pinMode(all_pins[i], OUTPUT);
  }

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

  iteration += 1;
}

void updateFrameState(long now) {
  long elapsed = now - frameStart;

  if (elapsed > microsecondsPerFrame) {
    frameStart = micros();
    frameNumber += 1;
    updateFrame();
  }
}

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

void updateFrame() {
  memset(frame, 0, sizeof(frame));

  int position = frameNumber % sizeof(frame);
  Serial.println(position);
  frame[position] = true;  
}

void rowShow(int rowNumber, bool pattern[]) {
  long thisStart = micros();

  allOff();
  digitalWrite(rows[rowNumber], HIGH);

  for (int i = 0; i < nCols; i++) {
    int position = rowNumber * nRows + i;
    if (pattern[position]) {
      digitalWrite(cols[i], HIGH);
    }
  }

  busyMicros += micros() - thisStart;
}


void allOff() {
  for (int i = 0; i < 8; i++) {
    digitalWrite(all_pins[i], LOW);
  }
}

void allOn() {
  for (int i = 0; i < 8; i++) {
    digitalWrite(all_pins[i], HIGH);
  }
}
