#include <Wire.h>

#define ADDRESS 97  // Slave; address randomly chosen

// Variables
const int bufferSize = 50;  // Define the size of your buffer
char buffer[bufferSize];    // Create the character buffer
char terminator = '\n';     // Define the terminator character
int bytesRead = 0;          // Variable to hold the number of bytes read


void setup() {
  Wire.begin();
  Serial.begin(115200);
}

void loop() {
  Serial.println("waiting");
  delay(666);

  if (Serial.available() > 0) {
    bytesRead = Serial.readBytesUntil('\n', buffer, bufferSize - 1);
    // Null-terminate the buffer to make it a valid C string
    buffer[bytesRead] = '\0';

    Serial.println(buffer);

    Wire.beginTransmission(ADDRESS);
    Wire.write(buffer);
    Wire.endTransmission();
  }
}
