//
// Created by SU on 21.05.2023.
//

#ifndef MICROCONTROLLER_PRESENCESENSOR_H
#define MICROCONTROLLER_PRESENCESENSOR_H

#endif //MICROCONTROLLER_PRESENCESENSOR_H

#include "Arduino.h"

struct PresenceSensor {
    const byte pin;

    explicit PresenceSensor(byte pin);

    void initialize() const;

    void setPresent(bool isPresent, unsigned long now);

    void setPresent(bool isPresent);

    bool isPresentNow();

    bool isPresentLong(unsigned long presenceMinDuration, bool* isPresentCurrently = nullptr);

    unsigned long getLastUpdate() const;
private:
    unsigned long lastUpdate = 0;
    unsigned long lastPresent = 0;
    unsigned long lastAbsent = 0;

    bool isPresentNow(unsigned long now);
};