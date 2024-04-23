//
// Created by SU on 18.05.2023.
//

#ifndef MICROCONTROLLER_TEMPERATUREDATA_H
#define MICROCONTROLLER_TEMPERATUREDATA_H

#include "WeatherData.h"

struct Weather {
    WeatherData outside;
    WeatherData inside;

    Weather() {};

    Weather(const WeatherData &outside, const WeatherData &inside)
        : outside(outside),
          inside(inside) {}
};

#endif //MICROCONTROLLER_TEMPERATUREDATA_H