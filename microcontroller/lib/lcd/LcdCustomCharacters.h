//
// Created by SU on 22.05.2023.
//

#ifndef LCD_CUSTOM_CHARACTERS_H
#define LCD_CUSTOM_CHARACTERS_H

#endif //LCD_CUSTOM_CHARACTERS_H

#include "Arduino.h"

const byte LCD_CHAR_CELSIUS_DEGREES = 0;
const char LCD_CHAR_CELSIUS_DEGREES_BYTES[] = {
        B11000,
        B11000,
        B00110,
        B01001,
        B01000,
        B01001,
        B00110,
        B00000
//        B01000,
//        B10100,
//        B01000,
//        B00110,
//        B01001,
//        B01000,
//        B01001,
//        B00110
//        B00000,
//        B10000,
//        B00110,
//        B01001,
//        B01000,
//        B01000,
//        B01001,
//        B00110
};

const byte LCD_CHAR_OUTSIDE = 1;
const char LCD_CHAR_OUTSIDE_BYTES[] = {
        B00000,
        B00000,
        B01110,
        B01010,
        B01010,
        B01110,
        B00000,
        B00000
};

const byte LCD_CHAR_INSIDE = 2;
const char LCD_CHAR_INSIDE_BYTES[] = {
        B00000,
        B00000,
        B01110,
        B00100,
        B00100,
        B01110,
        B00000,
        B00000
};
