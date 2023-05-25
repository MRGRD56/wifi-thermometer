//
// Created by SU on 18.05.2023.
//

#include "Ota.h"
#include <Arduino.h>
#include <ArduinoOTA.h>

const String EMPTY_STRING = "";

void initializeArduinoOta(void (*showMessage)(const String line1, const String line2)) {
    ArduinoOTA.onStart([&showMessage]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else {  // U_FS
            type = "fs";
        }

        // NOTE: if updating FS this would be the place to unmount FS using FS.end()
        Serial.println("Start updating " + type);
        showMessage("Updating " + type, EMPTY_STRING);
    });
    ArduinoOTA.onEnd([&showMessage]() {
        Serial.println("\nEnd");
        showMessage("Updated", EMPTY_STRING);
    });
    ArduinoOTA.onProgress([&showMessage](unsigned int progress, unsigned int total) {
        unsigned int percentage = progress / (total / 100);
        Serial.printf("Progress: %u%%\r", percentage);
        showMessage("Updating", String(percentage) + "%");
    });
    ArduinoOTA.onError([&showMessage](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Serial.println("Auth Failed");
            showMessage("Update Failed", "Auth Error");
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Begin Failed");
            showMessage("Update Failed", "Begin Error");
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Connect Failed");
            showMessage("Update Failed", "Connect Error");
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Receive Failed");
            showMessage("Update Failed", "Receive Error");
        } else if (error == OTA_END_ERROR) {
            Serial.println("End Failed");
            showMessage("Update Failed", "End Error");
        }
    });

    ArduinoOTA.begin();
}

void handleArduinoOta() {
    ArduinoOTA.handle();
}