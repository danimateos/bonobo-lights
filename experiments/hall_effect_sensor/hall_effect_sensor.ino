// Barduino

// Sensor is 44E 402 
// https://forum.arduino.cc/t/how-do-i-use-this-hall-sensor-44e-402/328142/5

// Connections
// Sensor - to GND
// Sensor center pin to Barduino's 3V3 
// 5k1 resistor between sensor S and center pin
// Sensor S to Barduino's pin 1
const int HALL_SENSOR_PIN = 1;
const int HZ = 200;
const int INTERFACE_HZ = 10;
long start, now, step;
int reading, detections;

void setup() {
  pinMode(HALL_SENSOR_PIN, INPUT);

  Serial.begin(115200);
  Serial.println("Ready to go!");
  start = millis();
  step = 0;
  detections = 0;
}

void loop() {
  now = millis();

  if (now - start > 1000.0 / HZ) {
    step += 1;
    start = now;

    read();

    if (step % (HZ / INTERFACE_HZ) == 0) {
      show();
    }


  }
}

void read() {
  reading = analogRead(HALL_SENSOR_PIN);
}

void show() {
  Serial.println(analogRead(HALL_SENSOR_PIN));
}