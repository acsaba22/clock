#ifndef MANCHESTER_TEST_H
#define MANCHESTER_TEST_H

#include <Arduino.h>
#include "util.h"
#include "manchester.h"

void masterSetup(byte ledPin, byte debugPin) {
    pinMode(0, INPUT);
    pinMode(1, INPUT);
    pinMode(2, INPUT);
    pinMode(ledPin, OUTPUT);
    pinMode(debugPin, OUTPUT);
}

void slaveSetup(byte ledPin, byte debugPin) {
    pinMode(ledPin, INPUT);
    pinMode(debugPin, OUTPUT);
}

void setupManchesterTest(Mode mode, byte ledPin, byte debugPin) {
    if (mode == MASTER) {
        masterSetup(ledPin, debugPin);
    } else {
        slaveSetup(ledPin, debugPin);
    }
}

void masterLoop(byte ledPin, byte debugPin) {
    digitalWrite(debugPin, 1);
    delay(recieveTimeout*2);
    digitalWrite(debugPin, 0);
    byte data[] = {0b11010011};
    send(ledPin, data, 1);
}

void slaveLoop(byte ledPin, byte debugPin) {
    Receiver receiver(ledPin);
    byte data[10];
    Receiver::Status s = receiver.Receive(data, 1);
    u8 blinkNum = s+2;
    if (s == Receiver::READOK && data[0] == 0b11010011) {
        blinkNum = 1;
    }
    for (u8 i=0; i<blinkNum; i++) {
        digitalWrite(debugPin, HIGH);
        delay(200);
        digitalWrite(debugPin, LOW);
        delay(200);
    }
    delay(700);
}


void loopManchesterTest(Mode mode, byte ledPin, byte debugPin) {
    if (mode == MASTER) {
        masterLoop(ledPin, debugPin);
    } else {
        slaveLoop(ledPin, debugPin);
    }
}

#endif // MANCHESTER_TEST_H
