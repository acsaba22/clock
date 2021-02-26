#ifndef DIGIT_H
#define DIGIT_H

#include "util.h";

const u8 LEDA = 0b00000010;
const u8 LEDB = 0b00010000;
const u8 LEDC = 0b00000001;
const u8 LEDD = 0b00100000;
const u8 LEDE = 0b01000000;
const u8 LEDF = 0b00000100;
const u8 LEDG = 0b10000000;
const u8 LEDH = 0b00001000;

u8 digitVal(byte d) {
    switch(d) {
    case 0:
        return ~(LEDA|LEDB|LEDC|LEDE|LEDF|LEDG);
    case 1:
        return ~(LEDC|LEDF);
    case 2:
        return ~(LEDA|LEDC|LEDD|LEDE|LEDG);
    case 3:
        return ~(LEDA|LEDC|LEDD|LEDF|LEDG);
    case 4:
        return ~(LEDB|LEDC|LEDD|LEDF);
    case 5:
        return ~(LEDA|LEDB|LEDD|LEDF|LEDG);
    case 6:
        return ~(LEDA|LEDB|LEDD|LEDE|LEDF|LEDG);
    case 7:
        return ~(LEDA|LEDC|LEDF);
    case 8:
        return ~(LEDA|LEDB|LEDC|LEDD|LEDE|LEDF|LEDG);
    case 9:
        return ~(LEDA|LEDB|LEDC|LEDD|LEDF|LEDG);
    default:
        return ~(LEDA|LEDB|LEDD|LEDE|LEDG|LEDH);
    }
}

#endif // DIGIT_H