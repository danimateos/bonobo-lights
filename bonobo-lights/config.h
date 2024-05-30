#include <stdint.h>
#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <FastLED.h>

#define NUMPIXELS 32  // Number of LEDs in strip
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

// UI Settings
#define SERIAL_UPDATE 500000  // print every SERIAL_UPDATE microseconds
#define frameRate 12          // For video
#define refreshRate 5000
#define HALL_MARGIN 40  // Lower for more sensitivity, higher for more accuracy. Between 0 and 500.
#define angularPixels 240


extern const int allOutputPins[];
extern long revolution;
extern bool detected;
extern bool previouslyDetected;
extern bool cleared;
extern long lastSerialPrint;
extern long step;
extern long now, sliceOnMicros, revolutionStart;
extern long currentDurationOfRevolution;
extern int polarIndex;
extern float angularResolution;

extern bool debug;

extern unsigned long microsecondsPerFrame;
extern unsigned long microsecondsPerRefresh;

// Framebuffer
extern bool primes[NUMPIXELS];
extern bool all[NUMPIXELS];
extern const uint8_t blankSlice[NUMPIXELS * 3];
extern uint8_t *slice;  
extern uint8_t scratchSlice[NUMPIXELS * 3];  // This is where we store a single slice in r g b r g b format, if it needs building.
extern uint8_t pattern[angularPixels * NUMPIXELS * 3];  // This is where we store the whole image to be shown in column-row-channel order
extern CRGB leds[NUMPIXELS];

#endif
