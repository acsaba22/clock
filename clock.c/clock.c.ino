#include <Arduino.h>
#include "util.h"

#include "manchester_test.h"
const TestMode mode = MASTER;
const byte ledPin = 3;
const byte debugPin = 4;

void setup() {
    setupManchesterTest(mode, ledPin, debugPin);
}

void loop() {
    loopManchesterTest(mode, ledPin, debugPin);
}
