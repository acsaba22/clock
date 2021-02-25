#ifndef UTIL_H
#define UTIL_H

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

#endif // UTIL_H
