//
// Created by SU on 23.04.2024.
//

#ifndef MICROCONTROLLER_WEATHERDATA_H
#define MICROCONTROLLER_WEATHERDATA_H

#include "WeatherType.h"

struct WeatherData {
    float temperature;
    float humidity;
    WeatherType weatherType;

    WeatherData() = default;

    WeatherData(float temperature, float humidity)
            : temperature(temperature),
              humidity(humidity),
              weatherType(CLEAR) {}

    WeatherData(float temperature, float humidity, WeatherType weatherData)
            : temperature(temperature),
              humidity(humidity),
              weatherType(weatherData) {}
};

#endif //MICROCONTROLLER_WEATHERDATA_H
