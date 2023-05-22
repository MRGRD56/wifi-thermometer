#ifndef MRGRD56_BUTTON_H
#define MRGRD56_BUTTON_H

#endif //MRGRD56_BUTTON_H

#include <Arduino.h>

struct Button {
public:
    const byte pin;

    explicit Button(byte pin) : pin(pin) {}

    void initialize() const {
        pinMode(pin, INPUT_PULLUP);
    }

    bool isPressed(unsigned long& millisPressed, unsigned long maxPressTime = -1) {
        state = digitalRead(pin);
        previousState = lastState;
        lastState = state;

        unsigned long now = millis();

        if (previousState == HIGH && state == LOW) { // just pressed
            pressStart = now;
            return false;
        }

        if (previousState == LOW && state == HIGH) { // just released
            if (isAlreadyPressed) {
                isAlreadyPressed = false;
                return false;
            }

            millisPressed = now - pressStart;
            return true;
        }

        if (state == LOW && maxPressTime != -1 && !isAlreadyPressed) {
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
    byte state = HIGH;
    byte previousState = HIGH;
    byte lastState = HIGH;
    unsigned long pressStart = 0;
    bool isAlreadyPressed = false;
};