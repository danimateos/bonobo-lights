#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "config.h"

static inline void fastWrite(int bitnum, int val);
void allOff();
void allOn();
void loadSlice(uint8_t newSlice[NUMPIXELS * 3]);
void loadSlice(bool newSlice[NUMPIXELS], uint32_t color);
void updateSensor();
void updateSpeedEstimation();
float currentPosition();
void updatePolarIndex(long now);
void showSlice();
void printSerial();
void printArray(uint8_t array[], int length);
void loadPattern(uint8_t* newPattern, int sizeOfNewPattern);
void takeUserInput();
void readI2C(int bytesToRead);

#endif
