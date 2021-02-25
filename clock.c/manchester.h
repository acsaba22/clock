#ifndef MANCHESTER_H
#define MANCHESTER_H

// https://en.wikipedia.org/wiki/Differential_Manchester_encoding

#include "util.h"


const u16 sendHalf = 100;
const u16 sendRate = sendHalf*2;
const u16 recieveTimeout = sendRate*3;

// don't change, assumed by logic.
const u16 initSignal = sendHalf + sendRate;

struct Sender {
    u8 pin;
    u8 currVal;
    u32 currStart;
    u32 toWait;

    Sender(u8 pin) {
        this->pin = pin;
    }

    void waitAndChange() {
        currVal = 1-currVal;
        while (true) {
            u32 now = micros();
            u32 elapsed = now-currStart;
            if (toWait < elapsed) {
                digitalWrite(pin, currVal);
                currStart += toWait;
                return;
            }
        }
    }

    void Send(byte* dataVec, byte n) {
        currVal = 0;
        digitalWrite(pin, currVal); // 0
        currStart = micros();
        toWait = sendRate;
        waitAndChange(); // 1
        toWait = initSignal;
        for (byte i=0; i<n; i++) {
            byte data = dataVec[i];
            for (byte bit = 0x80; bit; bit>>=1) {
                if (bit&data) {
                    waitAndChange();
                    toWait = sendRate;
                } else {
                    waitAndChange();
                    toWait = sendHalf;
                    waitAndChange();
                    toWait = sendHalf;
                }
            }
        }
        waitAndChange();
        if (currVal==1) {
            toWait = sendHalf;
            waitAndChange(); //0
        }
    }
};

struct Receiver {
    enum Status {
        READOK,
        ALREADYHIGH,
        BADCYCLELEN,
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

        u32 actualInitTime = prevLength;
        int diff = int(actualInitTime) - initSignal;
        diff = abs(diff);
        if ((25*initSignal)/100 < diff) {
            return BADCYCLELEN;
        }

        u32 longShortThreshold = actualInitTime / 2;
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