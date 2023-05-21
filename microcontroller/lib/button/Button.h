#ifndef MRGRD56_BUTTON_H
#define MRGRD56_BUTTON_H

#endif //MRGRD56_BUTTON_H

#include <Arduino.h>

struct Button {
  byte pin;
  byte state = HIGH;
  byte previousState = HIGH;
  byte lastState = HIGH;

  explicit Button(byte pin) : pin(pin) {}

  void initialize() {
    pinMode(pin, INPUT_PULLUP);
  }

  bool isPressed() {
    state = digitalRead(pin);
    previousState = lastState;
    lastState = state;

    return previousState == LOW && state == HIGH;
  }
};