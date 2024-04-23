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
};

const byte LCD_CHAR_INSIDE = 1;
const char LCD_CHAR_INSIDE_BYTES[] = {
        B00000,
        B00100,
        B01110,
        B11011,
        B10001,
        B10001,
        B11111,
        B00000
};

const byte LCD_CHAR_OUTSIDE_CLEAR = 2;
const char LCD_CHAR_OUTSIDE_CLEAR_BYTES[] = {
        B00000,
        B01110,
        B11111,
        B11111,
        B11111,
        B01110,
        B00000,
        B00000
};

const byte LCD_CHAR_OUTSIDE_FEW_CLOUDS = 3;
const char LCD_CHAR_OUTSIDE_FEW_CLOUDS_BYTES[] = {
        B00000,
        B00011,
        B01011,
        B11000,
        B11111,
        B01110,
        B00000,
        B00000
};

const byte LCD_CHAR_OUTSIDE_CLOUDY = 4;
const char LCD_CHAR_OUTSIDE_CLOUDY_BYTES[] = {
        B00000,
        B00000,
        B01100,
        B11111,
        B11111,
        B01110,
        B00000,
        B00000
};

const byte LCD_CHAR_OUTSIDE_PRECIPITATION = 5;
const char LCD_CHAR_OUTSIDE_PRECIPITATION_BYTES[] = {
        B01100,
        B11111,
        B11111,
        B01110,
        B00000,
        B10101,
        B01010,
        B10101
};

const byte LCD_CHAR_OUTSIDE_THUNDERSTORM = 6;
const char LCD_CHAR_OUTSIDE_THUNDERSTORM_BYTES[] = {
        B01100,
        B11111,
        B11111,
        B00000,
        B01110,
        B00110,
        B01100,
        B01000
};

const byte LCD_CHAR_OUTSIDE_MIST = 7;
const char LCD_CHAR_OUTSIDE_MIST_BYTES[] = {
        B00000,
        B01100,
        B00000,
        B11111,
        B00000,
        B01110,
        B00000,
        B00000
};

//const char LCD_CHAR_OUTSIDE_BYTES[] = {
//        B00000,
//        B00000,
//        B01110,
//        B01010,
//        B01010,
//        B01110,
//        B00000,
//        B00000
//};

//const char LCD_CHAR_INSIDE_BYTES[] = {
//        B00000,
//        B00000,
//        B01110,
//        B00100,
//        B00100,
//        B01110,
//        B00000,
//        B00000
//};