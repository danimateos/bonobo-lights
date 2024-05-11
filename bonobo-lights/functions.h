#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "config.h"

void someFunction();
static inline void fastWrite(int bitnum, int val);
void allOff();
void allOn();
void updateSlice(bool newSlice[NUMPIXELS]);

#endif
