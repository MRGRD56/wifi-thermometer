//
// Created by SU on 06.04.2024.
//

#ifndef MICROCONTROLLER_WIFICREDENTIALS_H
#define MICROCONTROLLER_WIFICREDENTIALS_H

struct __attribute__((packed)) WiFiCredentials {
    char ssid[32];
    char password[64];
};

#endif //MICROCONTROLLER_WIFICREDENTIALS_H
