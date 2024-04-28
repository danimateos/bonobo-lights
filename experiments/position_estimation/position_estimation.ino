// For position estimation with exponential smoothing. In microseconds.
// Initialize durations to very high values (10^8us) so that there's
// no graphical updates until after 5 revolutions.
const int nRevolutionsToConsider = 5;
const int weights[nRevolutionsToConsider] = { 315, 625, 1250, 2500, 5000 };  // most recent weighs heavier
const int sumOfWeights = 9690;
long revolutionDurations[nRevolutionsToConsider] = { 100000000, 100000000, 100000000, 100000000, 100000000 };
long currentDurationOfRevolution = 100000000;
long revolutionStart;

// Barduino
const int HALL_SENSOR_PIN_DIGITAL = 1;  // detection pulls down to 0
const int INDICATOR_PIN = 48;           // Integrated LED

// Settings
const int INTERFACE_UPDATE = 100000;  // microseconds


// State machine
long start, now, step;
int reading_d, detections;
long revolution = 0;
bool pressed = false;
bool previouslyPressed = false;
long lastInterfaceUpdate;

void setup() {
  pinMode(HALL_SENSOR_PIN_DIGITAL, INPUT);
  pinMode(INDICATOR_PIN, OUTPUT);

  Serial.begin(115200);

  start = micros();
  revolutionStart = start;
  lastInterfaceUpdate = start;
  step = 0;
  revolution = 0;
  pressed = true;
}

void loop() {
  now = micros();

  updateSensor();  // Every single turn of the loop
  printSerial();   // On a fixed schedule
  lightUp();

  step += 1;
}

void lightUp() {
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

void printSerial() {
  now = micros();
  if (now - lastInterfaceUpdate > INTERFACE_UPDATE) {
    Serial.printf("revolution:%d revolutionStart:%d current_postion:%.3f currentDurationOfRevolution:%d\n", revolution, revolutionStart, currentPosition(), currentDurationOfRevolution);
    lastInterfaceUpdate = now;
  }
}
