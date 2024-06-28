#include <Wire.h>


// Status indicator LED
#define LED_R 0
#define LED_G 1

// Human input
#define BUTTON_1 7
#define BUTTON_2 6

// Sensor
#define HALL 3

#define ADDRESS 97  // Slave; address randomly chosen

// Variables
const int bufferSize = 50;  // Define the size of your buffer
char buffer[bufferSize];    // Create the character buffer
bool pendingTransmission = false;

void setup() {
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);

  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);

  Serial.begin(115200);

  Wire.begin(ADDRESS);
  Wire.onReceive(receive);
  Wire.onRequest(respond);
}

void loop() {
  Serial.print(".");
  delay(1000);
  Serial.print(".");
  delay(1000);
  Serial.println(".");
  delay(1000);
}

void receive(int bytesToRead) {
  int i = 0;
  Serial.println("");
  while (Wire.available() && i < bytesToRead) {
    buffer[i] = Wire.read();  // Read each byte into the buffer
    i++;
  }


  buffer[bytesToRead] = '\0';
  pendingTransmission = true;

  Serial.print("Received through Wire: ");
  Serial.println(buffer);
}

void respond() {
  Serial.println("Master requested data");

  // Send the prepared response to the master
  if (pendingTransmission) {
    Wire.write(buffer);

    Serial.print("Sent to master: ");
    Serial.println(buffer);
  }

  memset(buffer, 0, bufferSize);
  pendingTransmission = false;
}
