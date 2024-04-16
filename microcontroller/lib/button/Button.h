#ifndef MRGRD56_BUTTON_H
#define MRGRD56_BUTTON_H

#endif //MRGRD56_BUTTON_H

#include <Arduino.h>

#define STATE_PRESSED HIGH
#define STATE_UNPRESSED LOW
// vise versa for INPUT_PULLUP

struct Button {
public:
    const byte pin;

    explicit Button(byte pin) : pin(pin) {}

    void initialize() const {
        pinMode(pin, INPUT);
    }

    bool isPressed(unsigned long& millisPressed, unsigned long maxPressTime = -1) {
        state = digitalRead(pin);
        previousState = lastState;
        lastState = state;

        unsigned long now = millis();

        if (previousState == STATE_UNPRESSED && state == STATE_PRESSED) { // just pressed
            pressStart = now;
            return false;
        }

        if (previousState == STATE_PRESSED && state == STATE_UNPRESSED) { // just released
            if (isAlreadyPressed) {
                isAlreadyPressed = false;
                return false;
            }

            millisPressed = now - pressStart;
            return true;
        }

        if (state == STATE_PRESSED && maxPressTime != -1 && !isAlreadyPressed) {
            unsigned long currentMillisPressed = now - pressStart;
            if (currentMillisPressed >= maxPressTime) {
                isAlreadyPressed = true;
                millisPressed = currentMillisPressed;
                return true;
            }
        }

        return false;
    }

private:
    byte state = STATE_UNPRESSED;
    byte previousState = STATE_UNPRESSED;
    byte lastState = STATE_UNPRESSED;
    unsigned long pressStart = 0;
    bool isAlreadyPressed = false;
};