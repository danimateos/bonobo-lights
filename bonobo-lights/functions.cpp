#include "functions.h"
#include "config.h"

void someFunction() {
    // Function implementation
    Serial.println(globalVariable);
}

int globalVariable = 42;  // Initialize global variable
