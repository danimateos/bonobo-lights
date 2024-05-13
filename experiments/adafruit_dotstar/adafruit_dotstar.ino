#include <Arduino.h>
#include <Adafruit_DotStar.h>

#define NUMPIXELS 16  // Number of LEDs in strip
#define STRIP_DATA 15
#define STRIP_CLOCK 14

// Status indicator LED
#define LED_R 0
#define LED_G 1
#define LED_B 2

// Human input
#define BUTTON_1 3
#define BUTTON_2 6

// Sensor
#define HALL 7

Adafruit_DotStar strip(NUMPIXELS, STRIP_DATA, STRIP_CLOCK, DOTSTAR_BGR);

const int allOutputPins[] = { STRIP_DATA, STRIP_CLOCK, LED_R, LED_G, LED_B };

bool primes[NUMPIXELS] = { false, true, true, false, true, false, true, false, false, false, true, false, true, false, false, false };

void setup() {
  for (int i = 0; i < 8; i++) {
    // pinsOut |= (1 << allOutputPins[i]);
    pinMode(allOutputPins[i], OUTPUT);
  }
  // PORT_IOBUS->Group[0].OUTSET.reg = pinsOut; // not working yet

  pinMode(HALL, INPUT);

  strip.begin();  // Initialize pins for output
  // strip.updatePins();  // Switch over to hardware SPI
  strip.show();  // Turn all LEDs off ASAP

  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, HIGH);

  Serial.begin(115200);
}

void loop() {
  strip.setPixelColor(15, 0x6600FF);
  strip.show();
  delay(500);
  strip.setPixelColor(15, 0x00FF22);
  strip.show();
  delay(500);
  
}
