//
// Created by SU on 18.05.2023.
//

#ifndef MICROCONTROLLER_TEMPERATUREDATA_H
#define MICROCONTROLLER_TEMPERATUREDATA_H

#endif //MICROCONTROLLER_TEMPERATUREDATA_H

struct TemperaturePlace {
    float temperature;
    float humidity;

    TemperaturePlace(float temperature, float humidity)
        : temperature(temperature),
          humidity(humidity) {}
};

struct TemperatureData {
    TemperaturePlace outside;
    TemperaturePlace inside;

    TemperatureData(const TemperaturePlace &outside, const TemperaturePlace &inside)
        : outside(outside),
          inside(inside) {}
};
