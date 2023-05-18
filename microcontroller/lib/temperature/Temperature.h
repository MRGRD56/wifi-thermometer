//
// Created by SU on 18.05.2023.
//

#ifndef MICROCONTROLLER_TEMPERATUREDATA_H
#define MICROCONTROLLER_TEMPERATUREDATA_H

#endif //MICROCONTROLLER_TEMPERATUREDATA_H

struct TemperaturePlace {
    float* outsideTemperature;
    float* outsideHumidity;

    TemperaturePlace(float* outsideTemperature, float* outsideHumidity)
        : outsideTemperature(outsideTemperature),
          outsideHumidity(outsideHumidity) {}
};

struct TemperatureData {
    TemperaturePlace outside;
    TemperaturePlace inside;

    TemperatureData(const TemperaturePlace &outside, const TemperaturePlace &inside)
        : outside(outside),
          inside(inside) {}
};
