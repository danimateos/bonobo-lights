#include <Wire.h>


// Status indicator LED
#define LED_R 0
#define LED_G 1
#define LED_B 2

// Human input
#define BUTTON_1 3
#define BUTTON_2 6

// Sensor
#define HALL 7

#define ADDRESS 97  // Slave; address randomly chosen

// Variables
const int bufferSize = 50;  // Define the size of your buffer
char buffer[bufferSize];    // Create the character buffer

void setup() {
  Serial.begin(115200);

  Wire.begin(ADDRESS);  
  Wire.onReceive(receive);
}

void loop() {
  Serial.print(".");
  delay(500);
}

void receive(int bytesToRead) {
  int i = 0;
  Serial.println("");
  while (Wire.available() && i < bytesToRead) {
    buffer[i] = Wire.read();  // Read each byte into the buffer
    i++;
  }
  Serial.println(buffer);
}