#ifndef SLAVE_H
#define SLAVE_H

#include "util.h"
#include "manchester.h"

void setupSlave() {
    pinMode(COMMUNICATION_PIN, INPUT);
    pinMode(1, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
}

void putState(u8 state) {
    for (u8 i=0; i<4; i++) {
        digitalWrite(i+1, (1<<i) & state ? HIGH : LOW);
    }
}

const int n = 4;

void loopSlave(SlaveNum slaveNum) {
    u8 states[n]={0b1111, 0b1111, 0b1111, 0b1111};
    u8 state = 0b1111;
    u8 nextState = 0;
    Receiver::Status status;

    Receiver receiver(COMMUNICATION_PIN);
    while (true) {
        for (int i=0; i<4; i++) {
            status = receiver.Receive(&nextState, 1);
            if (status == Receiver::READOK) {
                if (slaveNum == SECOND) {
                    nextState >>= 4;
                }
                states[i] = nextState;
            }
            delayMicroseconds(SLAVEDELAY);
            putState(states[i]);
        }
    }
}



#endif // SLAVE_H