//
// Created by SU on 23.04.2024.
//

#ifndef MICROCONTROLLER_CONFIG_H
#define MICROCONTROLLER_CONFIG_H

#include "WiFiCredentials.h"

struct __attribute__((packed)) Config {
    WiFiCredentials wifi;
};

#endif //MICROCONTROLLER_CONFIG_H
