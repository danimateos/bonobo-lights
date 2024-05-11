#ifndef CONFIG_H
#define CONFIG_H

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

// UI Settings
#define SERIAL_UPDATE 100000        // print every SERIAL_UPDATE microseconds
#define frameRate 12  // For video
#define refreshRate 5000
#define HALL_MARGIN 40  // Lower for more sensitivity, higher for more accuracy. Between 0 and 500.
#define sizeOfPattern 19

extern const int allOutputPins[];
// State machine. A frame is a superstate of which each row represents
// one of 4 distinct substates, but I'm managing them independently
extern long revolution;
extern bool detected;
extern bool previouslyDetected;
extern long lastSerialPrint;
extern long step;
extern long now, sliceOnMicros, revolutionStart;
extern long currentDurationOfRevolution;
extern int polarIndex;
extern float offset;
extern float angularResolution;

extern bool debug;

extern unsigned long microsecondsPerFrame;
extern unsigned long microsecondsPerRefresh;

// Framebuffer
extern bool primes[NUMPIXELS];
extern bool blank[NUMPIXELS];
extern bool slice[NUMPIXELS];
extern bool pattern[sizeOfPattern][NUMPIXELS];
extern Adafruit_DotStar strip;

#endif
