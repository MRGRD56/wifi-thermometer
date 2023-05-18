#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_HTU21DF.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "ESP8266WebServer.h"
#include "../lib/Credentials.h"
#include "../lib/ota/Ota.h"
#include "../lib/thermistor/Thermistor.h"
#include "../lib/temperature/Temperature.h"

const String API_KEY = "a9b43ee71309";
const String API_HOST = "http://192.168.0.132:8080";

ESP8266WebServer server = ESP8266WebServer(80);

Adafruit_HTU21DF htu = Adafruit_HTU21DF();
Thermistor thermistor = Thermistor(A0);

void setLedLight(boolean isEnabled) {
    if (isEnabled) {
        digitalWrite(LED_BUILTIN, LOW);
    } else {
        digitalWrite(LED_BUILTIN, HIGH);
    }
}

bool isWifiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void connectToWifi() {
    setLedLight(true);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to ");
    Serial.print(WIFI_SSID);
    Serial.println(" ...");

    int i = 0;
    while (!isWifiConnected()) {
        setLedLight(false);
        delay(70);
        setLedLight(true);
        delay(1000 - 70);
        Serial.print(++i);
        Serial.print(' ');
    }

    Serial.println('\n');
    Serial.println("Connection established!");
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());

    setLedLight(false);
}

TemperatureData getTemperature() {
    setLedLight(true);

    float outsideTemperature = htu.readTemperature();
    float outsideHumidity = htu.readHumidity();
    float insideTemperature = thermistor.getTemperature(); // TODO use getTemperatureAvg instead

    TemperatureData result = {
        {
            outsideTemperature,
            outsideHumidity
        },
        {
            insideTemperature,
            -1
        }
    };

    setLedLight(false);

    return result;
}

char responseBody[256];

void handleGetTemperature() {
    String apiKey = server.arg("key");
    if (API_KEY != nullptr && API_KEY != apiKey) {
        Serial.println("GET /data: 403 Forbidden - invalid api key provided");

        server.send(403, "plain/text", "Invalid api key provided");
        return;
    }

    TemperatureData data = getTemperature();

    snprintf(responseBody, sizeof(responseBody),
             R"({"outside":{"temperature":%.2f,"humidity":%.2f},"inside":{"temperature":%.2f}})",
             data.outside.temperature,
             data.outside.humidity,
             data.inside.temperature);

    server.send(200, "application/json", responseBody);

    Serial.print("GET /data: 200 OK - ");
    Serial.println(responseBody);
}

//char httpRequestBody[128];

//void updateTemperature() {
//    setLedLight(true);
//
//    float temperature = htu.readTemperature();
//    float humidity = htu.readHumidity();
//    float insideTemperature = thermistor.getTemperatureAvg();
//
//    Serial.printf("NEW_DATA temperature=%.2f humidity=%.2f insideTemperature=%.2f\n",
//                  temperature, humidity, insideTemperature);
//
//    if (!isWifiConnected()) {
//        connectToWifi();
//    }
//
//    WiFiClient client;
//    HTTPClient http;
//
//    http.begin(client, API_HOST + "/temperature");
//
//    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
//
//    snprintf(httpRequestBody, sizeof(httpRequestBody), "key=%s&temperature=%.2f&humidity=%.2f&temp2=%.2f", API_KEY,
//             temperature, humidity, insideTemperature);
//
//    http.POST(httpRequestBody);
//
//    http.end();
//
//    setLedLight(false);
//}

void setup() {
    initializeArduinoOta();

    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(9600);

    thermistor.initialize();

    if (!htu.begin()) {
        Serial.println("Check circuit. HTU21D not found!");
        delay(1000);
        EspClass::restart();
    }

    connectToWifi();

    server.on("/temperature", handleGetTemperature);
    server.enableCORS(true);
    server.begin();

    Serial.println("Initialized");

    setLedLight(true);
    delay(120);
    setLedLight(false);
    delay(80);
    setLedLight(true);
    delay(120);
    setLedLight(false);
}

void loop() {
    handleArduinoOta();
    server.handleClient();
}
