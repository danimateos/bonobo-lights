#include <Wire.h>

#define ADDRESS 97  // Slave; address randomly chosen

// Variables
const int bufferSize = 50;  // Define the size of your buffer
char buffer[bufferSize];    // Create the character buffer
char terminator = '\n';     // Define the terminator character
int bytesRead = 0;          // Variable to hold the number of bytes read
bool readPending = false;

void setup() {
  Wire.begin();
  Serial.begin(115200);
}

void loop() {
  Serial.print(".");
  delay(1000);
  Serial.print(".");
  delay(1000);
  Serial.println(".");
  delay(1000);

  if (Serial.available() > 0) {
    bytesRead = Serial.readBytesUntil('\n', buffer, bufferSize - 1);
    // Null-terminate the buffer to make it a valid C string
    buffer[bytesRead] = '\0';
    Serial.print("Read from Serial: ");
    Serial.println(buffer);

    Wire.beginTransmission(ADDRESS);
    Wire.write(buffer);
    Wire.endTransmission();
    memset(buffer, 0, bufferSize);
    readPending = true;
  }





  if (readPending) {
    Serial.println("Proceeding to request echo");
    Wire.requestFrom(ADDRESS, bytesRead);


    if (Wire.available()) {
      size_t i = 0;
      while (Wire.available()) {
        buffer[i++] = Wire.read();
      }
      // Null-terminate the received data to make it a valid C string
      buffer[i] = '\0';

      Serial.println("Received response:");
      Serial.println(buffer);
      // Clear the buffer after reading
      memset(buffer, 0, bufferSize);
      readPending = false;
    } else {
      Serial.println("No response received");
    }
  }

  delay(500);
}
