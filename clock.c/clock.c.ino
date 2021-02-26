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
    // for programming.
    for (int i=0; i<5; i++) {
        pinMode(i, INPUT);
    }
    delay(4000);

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

void clearOutputs() {
    digitalWrite(1, LOW);
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
}

void testDigits() {
    Sender sender(COMMUNICATION_PIN);
    u16 num = 0;
    u16 steps = 1;
    while (true) {
        u16 numRemainder = num;
        for (int k=0; k<4; k++) {
            u8 d = numRemainder % 10;
            numRemainder /= 10;
            u8 v = digitVal(d);
            sender.Send(&v, 1);
            clearOutputs();
            delayMicroseconds(SLAVEDELAY);
            digitalWrite(k+1, HIGH);
            // delay(300);
        }
        steps++;
        if (steps == 40) {
            steps = 0;
            num++;
            num %= 10000;
        }
    }
    // for (u8 d = 0; d<=10; d++) {
    //     u8 v = digitVal(d);
    //     sender.Send(&v, 1);
    //     delay(1000);
    // }
}

void loop() {
    if (mode == SLAVE) {
        loopSlave(slaveNum);
    } else {
        testDigits();
    }
}
