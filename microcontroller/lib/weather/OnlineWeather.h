//
// Created by SU on 23.04.2024.
//

#ifndef MICROCONTROLLER_ONLINEWEATHER_H
#define MICROCONTROLLER_ONLINEWEATHER_H

#include <HTTPClient.h>

#include <utility>
#include "ArduinoJson.h"

#include "../temperature/WeatherData.h"

class OnlineWeather {
public:
    explicit OnlineWeather(String apiKey)
            : apiKey(std::move(apiKey)),
              weatherData() {};

    WeatherData getWeather() {
        return weatherData;
    }

    bool fetchWeather(double lat, double lon) {
        Serial.println("Fetching weather");

        HTTPClient http;

        String requestUrl = String("https://api.openweathermap.org/data/2.5/weather?units=metric&lat=");
        requestUrl.concat(lat);
        requestUrl.concat("&lon=");
        requestUrl.concat(lon);
        requestUrl.concat("&appid=");
        requestUrl.concat(apiKey);

        http.begin(requestUrl);
        int status = http.GET();
        String bodyRaw = http.getString();
        http.end();

        if (status != 200) {
            Serial.printf("Got error %d from weather service %s\n", status, bodyRaw.c_str());
            return false;
        }

        JsonDocument body;
        deserializeJson(body, bodyRaw);

        JsonArray weather = body["weather"];

        WeatherType weatherType = CLEAR;
        if (weather.size() > 0) {
            int weatherId = weather[0]["id"];
            weatherType = convertWeatherType(weatherId);
        }

        JsonObject main = body["main"];

        float temperature = main["temp"];
        float humidity = main["humidity"];

        weatherData = WeatherData(temperature, humidity, weatherType);

        return true;
    }

private:
    String apiKey;
    WeatherData weatherData;

    static WeatherType convertWeatherType(int weatherId) {
        if (weatherId == 800) {
            return CLEAR;
        }
        if (weatherId >= 801 && weatherId < 900) {
            return CLOUDS;
        }
        if (weatherId >= 500 && weatherId < 600) {
            return RAIN;
        }
        if (weatherId >= 600 && weatherId < 700) {
            return SNOW;
        }
        if (weatherId >= 300 && weatherId < 400) {
            return DRIZZLE;
        }
        if (weatherId >= 200 && weatherId < 300) {
            return THUNDERSTORM;
        }
        if (weatherId >= 700 && weatherId < 800) {
            return ATMOSPHERE;
        }

        return CLEAR;
    }
};


#endif //MICROCONTROLLER_ONLINEWEATHER_H
