// Choose "tools" -> "Serial config" -> "ONE_UART_ONE_WIRE_TWO_SPI"
#include <Arduino.h>
#include <Adafruit_DotStar.h>
#include <wiring_private.h>

#define STRIP_DATA 14   // MOSI1 is defined as 14ul in variant.h,
#define STRIP_CLOCK 15  // SCK1 sane

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
// SPIClass sercom2SPI(&sercom2, -1, STRIP_DATA, STRIP_CLOCK, SPI_PAD_2_SCK_3, SERCOM_RX_PAD_0);


const int allOutputPins[] = { STRIP_DATA, STRIP_CLOCK, LED_R, LED_G, LED_B };


void setup() {
  for (int i = 0; i < 5; i++) {
    pinMode(allOutputPins[i], OUTPUT);
  }

  pinMode(HALL, INPUT);

  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, HIGH);

  Serial.begin(115200);

  SPI.begin();
  SPI1.begin();

}

void loop() {
  
SS
  Serial.println(SERCOM_RX_PAD_2RC)

  Serial.println(PIN_PA00);
  delay(1000);
}
