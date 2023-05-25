//
// Created by SU on 18.05.2023.
//

#ifndef MICROCONTROLLER_OTA_H
#define MICROCONTROLLER_OTA_H

#endif //MICROCONTROLLER_OTA_H

#include <Arduino.h>

void initializeArduinoOta(void (*showMessage)(const String line1, const String line2));

void handleArduinoOta();