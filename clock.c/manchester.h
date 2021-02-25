#ifndef MANCHESTER_H
#define MANCHESTER_H

// https://en.wikipedia.org/wiki/Differential_Manchester_encoding

#include "util.h"


const int sendHalf = 500;
const int sendRate = sendHalf*2;
const int recieveTimeout = sendRate*3;

void send(byte pin, byte* dataVec, byte n) {
    digitalWrite(pin, 0);
    delayMicroseconds(sendRate);
    digitalWrite(pin, 1);
    delayMicroseconds(sendRate);
    byte lastVal = 1;
    for (byte i=0; i<n; i++) {
        byte data = dataVec[i];
        for (byte bit = 0x80; bit; bit>>=1) {
            if (bit&data) {
                lastVal = 1 - lastVal;
                digitalWrite(pin, lastVal);
                delayMicroseconds(sendRate);
            } else {
                digitalWrite(pin, 1-lastVal);
                delayMicroseconds(sendHalf);
                digitalWrite(pin, lastVal);
                delayMicroseconds(sendHalf);
            }
        }
    }
    lastVal = 1-lastVal;
    digitalWrite(pin, lastVal);
    if (lastVal==1) {
        delayMicroseconds(sendHalf);
        digitalWrite(pin, 0);
    }
}

struct Receiver {
    enum Status {
        READOK,
        ALREADYHIGH,
        BADDELAY,
        TIMEOUT,
        SINGLESHORT,
    };

    u8 pin;
    u8 currVal;
    u32 currStart;
    u32 prevLength;

    Receiver(byte pin) {
        this->pin = pin;
    }

    Status waitChange() {
        u32 prevStart = currStart;
        while (digitalRead(pin) == currVal) {
            ulong elapsed = micros()-currStart;
            if (recieveTimeout < elapsed) {
                return TIMEOUT;
            }
        }
        currStart = micros();
        prevLength = currStart - prevStart;
        currVal = 1 - currVal;
        return READOK;
    }

    Status Receive(byte *dataVec, byte n) {
        if (digitalRead(pin) != 0) {
            return ALREADYHIGH;
        }
        while (digitalRead(pin) == 0) {}
        currStart = micros();
        currVal = 1;
        Status ret = waitChange();
        if (ret != READOK) {
            return ret;
        }
        u32 cycle_time = prevLength;
        int diff = int(cycle_time) - sendRate;
        diff = abs(diff);
        if ((3*sendRate)/100 < diff) {
            return BADDELAY;
        }
        u32 longShortThreshold = (cycle_time * 3) / 4;
        for (u8 i = 0; i<n; i++) {
            u8 data = 0;
            for (u8 bit = 0x80; bit; bit>>=1) {
                ret = waitChange();
                if (ret != READOK) {
                    return ret;
                }
                if (prevLength < longShortThreshold) {
                    // 0
                    ret = waitChange();
                    if (ret != READOK) {
                        return ret;
                    }
                    if (longShortThreshold <= prevLength) {
                        return SINGLESHORT;
                    }
                } else {
                    data |= bit;
                }
            }
            dataVec[i] = data;
        }
        return READOK;
    }
};


#endif// MANCHESTER_H