#include "functions.h"
#include "config.h"

void someFunction() {
    // Function implementation
    Serial.println(globalVariable);
}

const int allOutputPins[] = { STRIP_DATA, STRIP_CLOCK, LED_R, LED_G, LED_B };