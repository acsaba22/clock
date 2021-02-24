#include <Arduino.h>


#define LEN(a) (sizeof(a)/sizeof(*a))

enum Mode {
    MASTER,
    SLAVE
};

const byte mode = SLAVE;
const byte ledPin = 3;
const byte debugPin = 4;

typedef unsigned long ulong; // 32
typedef unsigned int uint; // 16
typedef unsigned long u32;
// typedef unsigned int u16;
typedef byte u8;
typedef long i32;
typedef int i16;
typedef char i8;

void masterSetup() {
    pinMode(ledPin, OUTPUT);
    pinMode(debugPin, OUTPUT);
}

void slaveSetup() {
    pinMode(ledPin, INPUT);
    pinMode(debugPin, OUTPUT);
}

void setup() {
    if (mode == MASTER) {
        masterSetup();
    } else {
        slaveSetup();
    }
}

const int sendHalf = 500;
const int sendRate = sendHalf*2;
const int recieveTimeout = sendRate*3;

void masterLoop() {
    digitalWrite(debugPin, 1);
    delay(recieveTimeout*2);
    digitalWrite(debugPin, 0);
    byte data[] = {0b11010011};
    send(ledPin, data, 1);
}



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
        delay(sendHalf);
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
            ulong elapsed = millis()-currStart;
            if (recieveTimeout < elapsed) {
                return TIMEOUT;
            }
        }
        currStart = millis();
        prevLength = currStart - prevStart;
        currVal = 1 - currVal;
        return READOK;
    }

    Status Receive(byte *dataVec, byte n) {
        if (digitalRead(pin) != 0) {
            return ALREADYHIGH;
        }
        while (digitalRead(pin) == 0) {}
        currStart = millis();
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
        u16 longShortThreshold = (cycle_time * 3) / 4;
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
        delay((u32(cycle_time)*9)*n);
        return READOK;
    }
};

// returns 0 if ok, error code otherwise

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

void slaveLoop() {
    Receiver receiver(ledPin);
    byte data[10];
    Receiver::Status s = receiver.Receive(data, 1);
    u8 blinkNum = s+1;
    for (u8 i=0; i<blinkNum; i++) {
        digitalWrite(debugPin, HIGH);
        delay(200);
        digitalWrite(debugPin, LOW);
        delay(200);
    }
    delay(400);
    // if d
    // byte data[] = {0b11010011};
}


void loop() {
    if (mode == MASTER) {
        masterLoop();
    } else {
        slaveLoop();
    }
}
