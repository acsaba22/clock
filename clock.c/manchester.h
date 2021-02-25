#ifndef MANCHESTER_H
#define MANCHESTER_H

// https://en.wikipedia.org/wiki/Differential_Manchester_encoding

#include "util.h"


const u16 sendHalf = 50;
const u16 sendRate = sendHalf*2;
const u16 recieveTimeout = sendRate*3;

// don't change, assumed by logic.
const u16 initSignal = sendHalf + sendRate;

struct Sender {
    u8 pin;
    u8 currVal;
    u16 currStart;
    u16 toWait;

    Sender(u8 pin) {
        this->pin = pin;
    }

    inline void waitAndChange() {
        currVal = 1-currVal;
        bool ok = false;
        while (true) {
            if (toWait <= u16(micros()) - u16(currStart)) {
                digitalWrite(pin, currVal);
                currStart += toWait;
                if (!ok) {
                    delay(10);
                }
                return;
            }
            ok = true;
        }
    }

    void Send2(byte* dataVec, byte n) {
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

    void Send(byte* dataVec, byte n) {
        currVal = 0;
        digitalWrite(pin, currVal); // 0
        delayMicroseconds(sendRate);
        currVal = 1 - currVal;
        digitalWrite(pin, currVal); // 1
        delayMicroseconds(initSignal-10);
        for (byte i=0; i<n; i++) {
            byte data = dataVec[i];
            for (byte bit = 0x80; bit; bit>>=1) {
                if (bit&data) {
                    currVal = 1 - currVal;
                    digitalWrite(pin, currVal); // 1
                    delayMicroseconds(sendRate-10);
                } else {
                    digitalWrite(pin, 1-currVal); // 1
                    delayMicroseconds(sendHalf);
                    digitalWrite(pin, currVal); // 1
                    delayMicroseconds(sendHalf-10);
                }
                if (bit!=1) {
                    delayMicroseconds(5);
                }
            }
        }
        currVal = 1 - currVal;
        digitalWrite(pin, currVal);
        if (currVal==1) {
            delayMicroseconds(sendHalf);
            digitalWrite(pin, 0);
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
        TOOSLOW
    };

    u8 pin;
    u8 currVal;
    u16 currStart;
    u16 prevLength;

    Receiver(byte pin) {
        this->pin = pin;
    }

    Status waitChange() {
        u16 prevStart = currStart;
        bool ok = false;
        while (digitalRead(pin) == currVal) {
            ok = true;
            u16 elapsed = u16(micros())-currStart;
            if (recieveTimeout < elapsed) {
                return TIMEOUT;
            }
        }
        if (!ok) {
            return TOOSLOW;
        }
        currStart = micros();
        prevLength = currStart - prevStart;
        currVal = 1 - currVal;
        return READOK;
    }

    Status Receive(byte *dataVec, byte n) {
        u16 longShortThreshold = initSignal / 2;
        i16 badCycleDiffLimit = (25*initSignal)/100;
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

        i16 actualInitTime = prevLength;
        i16 diff = actualInitTime - initSignal;
        if (badCycleDiffLimit < abs(diff)) {
            return BADCYCLELEN;
        }

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