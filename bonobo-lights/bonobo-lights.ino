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
bool pattern[nRows * nCols];

bool primes[nRows * nCols] = { false, true, true, false, true, false, true, false, false, false, true, false, true, false, false, false };

long iteration = 0;
long start, end;
int nCycles = 1000;

void setup() {
  for (int i = 0; i < 8; i++) {
    pinMode(all_pins[i], OUTPUT);
  }

  Serial.begin(115200);

  // See p 378 of the datasheet
  PORT_IOBUS->Group[0].OUTCLR;  // https://forum.arduino.cc/t/what-is-the-fastest-way-to-read-write-gpios-on-samd21-boards/907133/9
  start = micros();
  Serial.println("Let us play");
}

void loop() {

  rowShow(0, primes);

  if (iteration % nCycles == 0) {
    end = micros();

    long averageTime = (end - start) / nCycles;
    Serial.println(averageTime);
    start = micros();
  }
  iteration += 1;
}

void rowShow(int rowNumber, bool pattern[]) {
  allOff();
  digitalWrite(rows[rowNumber], HIGH);

  for (int i = 0; i < nCols; i++) {
    int position =  rowNumber * nRows + i;
    if (pattern[position]) {
      digitalWrite(cols[i], HIGH);
    }
  }

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
