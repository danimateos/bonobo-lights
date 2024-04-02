// Barduino

// Sensor is 44E 402
// https://forum.arduino.cc/t/how-do-i-use-this-hall-sensor-44e-402/328142/5

// Connections
// Sensor - to GND
// Sensor center pin to board 02's 3V3
// 10k resistor between sensor S and center pin
// Sensor S to board 02's pin 08
const int HALL_SENSOR_PIN_D = 8;   // detection pulls down to 0
const int HALL_SENSOR_PIN_A = 20;  // Why don't I manage to read this?
const int INDICATOR_PIN = 26;      // Why don't I manage to light this up?
const int HZ = 200;
const int INTERFACE_HZ = 20;  // Needs to be an even split of HZ
long start, now, step;
int reading_d, reading_a, detections;

void setup() {
  for (int i = 0; i < 23; i++) {
    pinMode(i, INPUT);
  }
  pinMode(INDICATOR_PIN, OUTPUT);

  Serial.begin(115200);
  Serial.println("Ready to go!");

  start = millis();
  step = 0;
  detections = 0;

  digitalWrite(INDICATOR_PIN, HIGH);
}

void loop() {
  now = millis();

  read();
  // detections += reading;

  if (now - start > 1000.0 / HZ) {
    step += 1;
    start = now;

    if (step % (HZ / INTERFACE_HZ) == 0) {
      show();
      detections = 0;
    }
  }
}

void read() {
  reading_d = analogRead(HALL_SENSOR_PIN_D);
  digitalWrite(INDICATOR_PIN, reading_d);
  reading_a = analogRead(HALL_SENSOR_PIN_A);
}

void show() {
  Serial.print(String("Reading_analog:") + reading_a);
  Serial.println(String(" Reading_digital:") + reading_d);
}