#include <WiFi.h>
#include <Wire.h>
#include "aWOT.h"
#include "StaticFiles.h"


// Wifi and frontend-related
#define WIFI_SSID "Iaac-Wifi"
#define WIFI_PASSWORD "EnterIaac22@"

WiFiServer server(80);
Application app;

// I2C communication with spoke unit
#define ADDRESS 97  // Slave; address randomly chosen

const int bufferSize = 50;  // Define the size of your buffer
char buffer[bufferSize];    // Create the character buffer
char terminator = '\n';     // Define the terminator character
int bytesRead = 0;          // Variable to hold the number of bytes read

// Temporary for interface testing
#define LED_PIN 8

void setup() {
  // Wire.begin();
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  app.use(staticFiles());
  app.get("/toggle", &toggleLED);

  server.begin();

}

void loop() {
  WiFiClient client = server.available();

  if (client.connected()) {
    app.process(&client);
  }

  // Write to SAMD21 through I2C
  if (Serial.available() > 0) {
    bytesRead = Serial.readBytesUntil('\n', buffer, bufferSize - 1);
    // Null-terminate the buffer to make it a valid C string
    buffer[bytesRead] = '\0';

    // transmitToSpoke(ADDRESS, buffer);
  }

}

void toggleLED(Request &req, Response &res){
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  Serial.println("Toggle");
}


void transmitToSpoke(uint8_t address, char buffer[]) {
  Serial.println("Sending through I2C:");
  Serial.println(buffer);


  Wire.beginTransmission(address);
  Wire.write(buffer);
  Wire.endTransmission();

  Serial.println("Done");
}