#ifndef UTIL_H
#define UTIL_H

#include <Arduino.h>

#define LEN(a) (sizeof(a)/sizeof(*a))

typedef unsigned long ulong; // 32
typedef unsigned int uint; // 16
typedef unsigned long u32;
// typedef unsigned int u16;
typedef byte u8;
typedef long i32;
typedef int i16;
typedef char i8;

enum Mode {
    MASTER,
    SLAVE
};

enum SlaveNum {
    FIRST,
    SECOND
};


// don't change, logic assumes this.
const u8 COMMUNICATION_PIN = 0;

const u16 SLAVEDELAY = 300;

#endif // UTIL_H
