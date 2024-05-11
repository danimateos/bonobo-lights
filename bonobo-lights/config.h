#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

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

extern int globalVariable;
extern const int allOutputPins[];

#endif
