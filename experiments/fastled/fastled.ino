#include <Arduino.h>
// #define ARDUINO_SAMD_ZERO // tried this as per https://www.reddit.com/r/FastLED/comments/jxrffn/fastled_on_seeeduino_cortex_m0_no_hardware_spi/, but it didn't have an effect; it was already defined by default.
#include <FastLED.h>

#define NUMPIXELS 16  // Number of LEDs in strip
#define STRIP_DATA PIN_SPI1_MOSI
#define STRIP_CLOCK PIN_SPI1_SCK

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

  // FastLED.addLeds<APA102, STRIP_DATA, STRIP_CLOCK, RGB>(leds, NUMPIXELS);
  FastLED.addLeds<DOTSTAR, STRIP_DATA, STRIP_CLOCK, RGB, DATA_RATE_MHZ(24)>(leds, NUMPIXELS);
  
   
  FastLED.setBrightness(50);

  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, HIGH);

  Serial.begin(115200);
}

void loop() {
  Serial.println(ARDUINO_SAMD_PEPINO);  // defined in boards.txt of the SAM core: x21e.build.board=SAMD_PEPINO
  
// Needs the following entry in libraries/FastLED/src/platforms/arm/d21/fastpin_arm_d21.h:
// #elif defined(ARDUINO_SAMD_PEPINO)
// #define MAX_PIN 31
// _FL_DEFPIN( 0, 0,0); _FL_DEFPIN( 1, 1,0); _FL_DEFPIN( 2, 2,0); _FL_DEFPIN( 3, 3,0);
// _FL_DEFPIN( 4, 4,0); _FL_DEFPIN( 5, 5,0); _FL_DEFPIN( 6, 6,0); _FL_DEFPIN( 7, 7,0);
// _FL_DEFPIN( 8, 8,0); _FL_DEFPIN( 9, 9,0); _FL_DEFPIN(10,10,0); _FL_DEFPIN(11,11,0);
// _FL_DEFPIN(12,12,0); _FL_DEFPIN(13,13,0); _FL_DEFPIN(14,14,0); _FL_DEFPIN(15,15,0);
// _FL_DEFPIN(16,16,0); _FL_DEFPIN(17,17,0); _FL_DEFPIN(18,18,0); _FL_DEFPIN(19,19,0);
// _FL_DEFPIN(20,20,0); _FL_DEFPIN(21,21,0); _FL_DEFPIN(22,22,0); _FL_DEFPIN(23,23,0);
// _FL_DEFPIN(24,24,0); _FL_DEFPIN(25,25,0); _FL_DEFPIN(26,26,0); _FL_DEFPIN(27,27,0);
// _FL_DEFPIN(28,28,0); _FL_DEFPIN(29,29,0); _FL_DEFPIN(30,30,0); _FL_DEFPIN(31,31,0);

// #define SPI_DATA 14
// #define SPI_CLOCK 15

// #define HAS_HARDWARE_PIN_SUPPORT 1


  leds[0] = CRGB::Red;
  long now = micros();
  FastLED.show();
  Serial.println(micros() - now);
  Serial.println("ON");
  delay(500);
  leds[0] = CRGB::Black;
  FastLED.show();
  Serial.println("OFF");
  delay(500);
}
