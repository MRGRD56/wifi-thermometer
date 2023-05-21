//
// Created by SU on 21.05.2023.
//

#include "PresenceSensor.h"

PresenceSensor::PresenceSensor(byte pin) : pin(pin) {}

void PresenceSensor::initialize() const {
    pinMode(pin, INPUT);
}

void PresenceSensor::setPresent(bool isPresent, unsigned long now) {
    lastUpdate = now;

    if (isPresent) {
        this->lastPresent = now;
    } else {
        this->lastAbsent = now;
    }
}

void PresenceSensor::setPresent(bool isPresent) {
    setPresent(isPresent, millis());
}

bool PresenceSensor::isPresentNow(unsigned long now) {
    bool isPresent = digitalRead(pin);

    setPresent(isPresent, now);

    return isPresent;
}

bool PresenceSensor::isPresentNow() {
    return isPresentNow(millis());
}

bool PresenceSensor::isPresentLong(unsigned long presenceMinDuration, bool* isPresentCurrently) {
    unsigned long now = millis();

    bool currentIsPresent = isPresentNow(now);

    if (isPresentCurrently) {
        *isPresentCurrently = currentIsPresent;
    }

    return this->lastPresent != 0 && (now - this->lastPresent) < presenceMinDuration;
}

unsigned long PresenceSensor::getLastUpdate() const {
    return lastUpdate;
}
