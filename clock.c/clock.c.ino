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

void loop() {
    digitalWrite(debugPin, 1);
    delay(1000);
    digitalWrite(debugPin, 0);
    byte data[] = {0b11010011};
    send(ledPin, data, 1);
}

const int sendHalf = 500;
const int sendRate = sendHalf*2;

void send(byte pin, byte* dataVec, byte n) {
    digitalWrite(pin, 0);
    delay(sendRate);
    digitalWrite(pin, 1);
    delay(sendRate);
    byte lastVal = 1;
    for (byte i=0; i<n; i++) {
        byte data = dataVec[i];
        for (byte bit = 0x80; bit; bit>>=1) {
            if (bit&data) {
                lastVal = 1 - lastVal;
                digitalWrite(pin, lastVal);
                delay(sendRate);
            } else {
                digitalWrite(pin, 1-lastVal);
                delay(sendHalf);
                digitalWrite(pin, lastVal);
                delay(sendHalf);
            }
        }
    }
    lastVal = 1-lastVal;
    digitalWrite(pin, lastVal);
    if (lastVal==1) {
        delay(sendRate);
        digitalWrite(pin, 0);
    }
}

enum ReadError {
    READOK,
    ALREADY,
    DELAY99,
    DELAY90,
    DELAY80
};

// returns 0 if ok, error code otherwise
bool recieve(byte pin, byte *dataVec, byte n) {
    if (digitalRead(pin) != 0) {
        return false;
    }
    while (digitalRead(pin) == 0) {}
    ulong start = millis();
    while (digitalRead(pin) == 1) {}
    ulong end = millis();
    // ulong cycle_time == 
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

const ulong cycles[] = {33000, 10000, 3300};

void TestCycleTimes() {
    for (int i=0; i<LEN(cycles); i++) {
        KeepOn(5000, cycles[i], 10);
    }
}

const byte fractions[] = {4, 8, 16, 32, 64};

void TestFractions() {
    for (int i=0; i<LEN(fractions); i++) {
        KeepOn(5000, 10000, fractions[i]);
    }
}
