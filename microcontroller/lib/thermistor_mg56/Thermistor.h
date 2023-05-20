//
// Created by SU on 18.05.2023.
//

#ifndef MICROCONTROLLER_THERMISTOR_H
#define MICROCONTROLLER_THERMISTOR_H

#endif //MICROCONTROLLER_THERMISTOR_H

#include <Arduino.h>

struct Thermistor {
private:
    static constexpr double VCC = 3.3;              // NodeMCU on board 3.3v vcc
    static constexpr double R2 = 10'000;            // 10k ohm series resistor
    static constexpr double ADC_RESOLUTION = 1023;  // 10-bit adc

    static constexpr double A = 0.001129148;        // thermistor_mg56 equation parameters
    static constexpr double B = 0.000234125;
    static constexpr double C = 0.0000000876741;

    static constexpr float KELVIN_TO_CELSIUS = 273.15f;

    static const int MAX_LAST_VALUES = 3;

    float lastValues[MAX_LAST_VALUES] = {};
    int lastValuesCount = 0;

public:
    const uint8_t pin;

    explicit Thermistor(const uint8_t pin) : pin(pin) {}

    void initialize() const {
        // noop
    }

    float getTemperature() const {
        double adcValue = (double) analogRead(pin);
        double vOut = (adcValue * VCC) / ADC_RESOLUTION;
        double Rth = (VCC * R2 / vOut) - R2;

        float temperatureKelvins = (float) (1 / (A + (B * log(Rth)) + (C * pow((log(Rth)), 3))));

        return temperatureKelvins - KELVIN_TO_CELSIUS;
    }

    float getTemperatureAvg() {
        float currentTemperature = getTemperature();

        float averageTemperature = currentTemperature;

        for (int i = 0; i < lastValuesCount; i++) {
            averageTemperature += lastValues[i];
        }

        averageTemperature /= (float) (lastValuesCount + 1);

        Serial.printf("T_AVG lastValues=[%.2f, %.2f, %.2f], current=%.2f ==> avg%.2f\n",
                      lastValues[0], lastValues[1], lastValues[2], currentTemperature, averageTemperature);

        if (lastValuesCount < MAX_LAST_VALUES) {
            lastValuesCount++;
        } else {
            for (int i = 1; i < lastValuesCount; i++) {
                lastValues[i - 1] = lastValues[i];
            }
        }

        lastValues[lastValuesCount - 1] = currentTemperature;

        return averageTemperature;
    }
};