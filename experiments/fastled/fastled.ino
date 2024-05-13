#include <Arduino.h>
#include <FastLED.h>

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

CRGB leds[NUMPIXELS];

const int allOutputPins[] = { STRIP_DATA, STRIP_CLOCK, LED_R, LED_G, LED_B };

void setup() {
  for (int i = 0; i < 5; i++) {
    pinMode(allOutputPins[i], OUTPUT);
  }

  pinMode(HALL, INPUT);

  // FastLED.addLeds<DOTSTAR, STRIP_DATA, STRIP_CLOCK, RGB>(leds, NUMPIXELS);
  FastLED.addLeds<DOTSTAR, STRIP_DATA, STRIP_CLOCK, RGB, DATA_RATE_MHZ(12)>(leds, NUMPIXELS);
  
  FastLED.setBrightness(50);

  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, HIGH);

  Serial.begin(115200);
}

void loop() {
  leds[15] = CRGB::Red;
  FastLED.show();
  Serial.println("ON");
  delay(500);
  leds[15] = CRGB::Black;
  FastLED.show();
  Serial.println("OFF");
  delay(500);
}
