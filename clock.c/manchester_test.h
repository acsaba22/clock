#ifndef MANCHESTER_TEST_H
#define MANCHESTER_TEST_H

#include <Arduino.h>
#include "util.h"
#include "manchester.h"

enum TestMode {
    MASTER,
    SLAVE
};

void testSetupMaster(byte ledPin, byte debugPin) {
    // pinMode(0, INPUT);
    // pinMode(1, INPUT);
    // pinMode(2, INPUT);
    pinMode(ledPin, OUTPUT);
    pinMode(debugPin, OUTPUT);
}

void testSetupSlave(byte ledPin, byte debugPin) {
    pinMode(ledPin, INPUT);
    pinMode(debugPin, OUTPUT);
}

void setupManchesterTest(TestMode mode, byte ledPin, byte debugPin) {
    if (mode == MASTER) {
        testSetupMaster(ledPin, debugPin);
    } else {
        testSetupSlave(ledPin, debugPin);
    }
}

const char message[] = "Hello World!";
const char messageLen = LEN(message);

void testLoopMaster(byte ledPin, byte debugPin) {
    digitalWrite(debugPin, 1);
    delay(2000);
    digitalWrite(debugPin, 0);
    Sender sender(ledPin);
    sender.Send((byte*)message, messageLen);
}

void tasteLoopSlave(byte ledPin, byte debugPin) {
    Receiver receiver(ledPin);
    byte data[messageLen+10];
    Receiver::Status s = receiver.Receive(data, messageLen);
    u8 blinkNum = s+2;
    if (s == Receiver::READOK && strncmp((char*)data, message, messageLen) == 0) {
        blinkNum = 1;
    }
    u16 ontime = 100;
    u16 offtime = 200;
    if (s == Receiver::TOOSLOW) {
        ontime = 50;
        offtime = 50;
    }
    for (u8 i=0; i<blinkNum; i++) {
        digitalWrite(debugPin, HIGH);
        delay(ontime);
        digitalWrite(debugPin, LOW);
        delay(offtime);
    }
    delay(400);
}


void loopManchesterTest(TestMode mode, byte ledPin, byte debugPin) {
    if (mode == MASTER) {
        testLoopMaster(ledPin, debugPin);
    } else {
        tasteLoopSlave(ledPin, debugPin);
    }
}

#endif // MANCHESTER_TEST_H
