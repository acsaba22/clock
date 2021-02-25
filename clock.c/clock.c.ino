#include <Arduino.h>
#include "util.h"
#include "manchester.h"
#include "manchester_test.h"

const Mode mode = SLAVE;
const byte ledPin = 3;
const byte debugPin = 4;

void setup() {
    setupManchesterTest(mode, ledPin, debugPin);
}

void loop() {
    loopManchesterTest(mode, ledPin, debugPin);
}
