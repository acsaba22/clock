#include "util.h"
#include "slave.h"
#include "digit.h"

// #include "manchester_test.h"
// const TestMode mode = MASTER;
// const byte ledPin = 3;
// const byte debugPin = 4;

const Mode mode = MASTER;
const SlaveNum slaveNum = SECOND;

const u8 debugPin = 4;

void setupMaster() {
    pinMode(COMMUNICATION_PIN, OUTPUT);
    pinMode(1, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
}

void setup() {
    if (mode == SLAVE) {
        setupSlave();
    } else if (mode == MASTER) {
        setupMaster();
    }
}

void testRound() {
    Sender sender(COMMUNICATION_PIN);
    for (u8 v = 0x01; v; v<<=1) {
        u8 neg = ~v;
        sender.Send(&neg, 1);
        delay(300);
    }
    delay(500);
}

void testDigits() {
    Sender sender(COMMUNICATION_PIN);
    // for (int k=0; k<10000; k++) {
    //     for (int d=0; d<4; d++) {
    //         digitalWrite(d+1, HIGH);
    //     }
    // }
    digitalWrite(1, HIGH);
    digitalWrite(2, HIGH);
    digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
    for (u8 d = 0; d<=10; d++) {
        u8 v = digitVal(d);
        sender.Send(&v, 1);
        delay(1000);
    }
}

void loop() {
    if (mode == SLAVE) {
        loopSlave(slaveNum);
    } else {
        testDigits();
    }
}
