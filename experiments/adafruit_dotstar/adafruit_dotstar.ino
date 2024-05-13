#include <Arduino.h>
#include <Adafruit_DotStar.h>
#include <wiring_private.h>

#define NUMPIXELS 16    // Number of LEDs in strip
#define STRIP_DATA 15   // MOSI1 is defined as 14ul in variant.h, but software SPI doesn't work when I switch up the pins!! Why the hell no?.
#define STRIP_CLOCK 14  // SCK1 sane

// Status indicator LED
#define LED_R 0
#define LED_G 1
#define LED_B 2

// Human input
#define BUTTON_1 3
#define BUTTON_2 6

// Sensor
#define HALL 7


// Setting up sercom 2 for hardware SPI.
// Pads 0 and 1 are not a pin in SAMD21E18A.
SPIClass sercom2SPI(&sercom2, -1, STRIP_DATA, STRIP_CLOCK, SPI_PAD_2_SCK_3, SERCOM_RX_PAD_0);

Adafruit_DotStar strip(NUMPIXELS, STRIP_DATA, STRIP_CLOCK, DOTSTAR_BGR);  // Software SPI (???)
// Adafruit_DotStar strip(NUMPIXELS, DOTSTAR_BGR, &sercom2SPI); // Hardware SPI (??)

const int allOutputPins[] = { STRIP_DATA, STRIP_CLOCK, LED_R, LED_G, LED_B };



void setup() {
  for (int i = 0; i < 5; i++) {
    pinMode(allOutputPins[i], OUTPUT);
  }

  pinPeripheral(STRIP_CLOCK, PIO_SERCOM);  // needs #include <wiring_private.h>
  pinPeripheral(STRIP_DATA, PIO_SERCOM);   // needs #include <wiring_private.h>


  pinMode(HALL, INPUT);

  strip.begin();  // Initialize pins for output
  // strip.updatePins();  // Switch over to hardware SPI
  strip.show();  // Turn all LEDs off ASAP
  strip.setBrightness(20);

  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, HIGH);

  Serial.begin(115200);
}

void loop() {
  long now = micros();
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, i * 0x06000F);
  }
  strip.show();
  Serial.println(micros() - now);
  delay(500);
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, 0);
  }
  strip.show();
  delay(500);
  Serial.println(SPI_INTERFACES_COUNT);  // Tools menu-> Serial config
}
