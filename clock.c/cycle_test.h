#ifndef TEST_CYCLE_H
#define TEST_CYCLE_H

#include <Arduino.h>
#include "util.h"

void blink(byte ledPin, byte debugPin) {
  digitalWrite(ledPin, HIGH);
  digitalWrite(debugPin, LOW);
  delay(200);
  digitalWrite(ledPin, LOW);
  digitalWrite(debugPin, HIGH);
  delay(1000);
}

void KeepOn(uint totalMilli, ulong cycleMicro, byte onFraction, byte ledPin, byte debugPin) {
    ulong onTimeMicro = cycleMicro / onFraction;
    ulong offTimeMicro = cycleMicro - onTimeMicro;
    bool milliMode = false;
    if (50000 < cycleMicro) {
        milliMode = true;
        onTimeMicro /= 1000;
        offTimeMicro /= 1000;
    }
    ulong start = millis();
    ulong end = start + totalMilli;
    ulong debugEnd = start + 300;
    bool debug = true;
    digitalWrite(debugPin, HIGH);
    for (ulong m = millis(); m < end; m = millis()) {
        if (debug == true && debugEnd <= m) {
            debug = false;
            digitalWrite(debugPin, LOW);
        }
        digitalWrite(ledPin, LOW);
        if (milliMode) {
            delay(onTimeMicro);
        } else {
            delayMicroseconds(onTimeMicro);
        }
        digitalWrite(ledPin, HIGH);
        if (milliMode) {
            delay(offTimeMicro);
        } else {
            delayMicroseconds(offTimeMicro);
        }
    }

}

// 10 millisec is good enough.
const ulong cycles[] = {33000, 10000, 3300};

void TestCycleTimes(byte ledPin, byte debugPin) {
    for (int i=0; i<LEN(cycles); i++) {
        KeepOn(5000, cycles[i], 10, ledPin, debugPin);
    }
}

// Any fraction is good. 1/64, is a bit pale.
const byte fractions[] = {4, 8, 16, 32, 64};

void TestFractions(byte ledPin, byte debugPin) {
    for (int i=0; i<LEN(fractions); i++) {
        KeepOn(5000, 10000, fractions[i], ledPin, debugPin);
    }
}

#endif // TEST_CYCLE_H
