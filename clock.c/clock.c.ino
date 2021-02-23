#include <Arduino.h>


#define LEN(a) (sizeof(a)/sizeof(*a))

enum Mode {
    MASTER,
    SLAVE
};

const byte mode = MASTER;
const byte ledPin = 3;
const byte debugPin = 4;

typedef unsigned long ulong;
typedef unsigned int uint;

void setup() {
    pinMode(ledPin, OUTPUT);
    pinMode(debugPin, OUTPUT);
}

void blink() {
  digitalWrite(ledPin, HIGH);
  digitalWrite(debugPin, LOW);
  delay(200);
  digitalWrite(ledPin, LOW);
  digitalWrite(debugPin, HIGH);
  delay(1000);

}

void KeepOn(uint totalMilli, ulong cycleMicro, byte onFraction) {
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
        digitalWrite(ledPin, HIGH);
        if (milliMode) {
            delay(onTimeMicro);
        } else {
            delayMicroseconds(onTimeMicro);
        }
        digitalWrite(ledPin, LOW);
        if (milliMode) {
            delay(offTimeMicro);
        } else {
            delayMicroseconds(offTimeMicro);
        }
    }

}

const ulong cycles[] = {1000000, 330000, 100000, 33000, 10000, 3300, 1000};

void TestOnTimes() {
    for (int i=0; i<LEN(cycles); i++) {
        KeepOn(2000, cycles[i], 4);
    }
}

void loop() {
    TestOnTimes();
}
