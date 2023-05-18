#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_HTU21DF.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "ESP8266WebServer.h"
#include "../lib/Credentials.h"
#include "../lib/ota/Ota.h"
#include "../lib/thermistor/Thermistor.h"

const char* API_KEY = "a9b43ee71309";
const String API_HOST = "http://192.168.0.132:8080";

Adafruit_HTU21DF htu = Adafruit_HTU21DF();
Thermistor thermistor = Thermistor(A0);

bool isWifiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void connectToWifi() {
    digitalWrite(LED_BUILTIN, LOW);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to ");
    Serial.print(WIFI_SSID);
    Serial.println(" ...");

    int i = 0;
    while (!isWifiConnected()) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(70);
        digitalWrite(LED_BUILTIN, LOW);
        delay(1000 - 70);
        Serial.print(++i);
        Serial.print(' ');
    }

    Serial.println('\n');
    Serial.println("Connection established!");
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());

    digitalWrite(LED_BUILTIN, HIGH);
}

char httpRequestBody[128];

void updateTemperature() {
    digitalWrite(LED_BUILTIN, LOW);

    float outsideTemperature = htu.readTemperature();
    float outsideHumidity = htu.readHumidity();
    float insideTemperature = thermistor.getTemperatureAvg();

    Serial.printf("NEW_DATA outsideTemperature=%.2f outsideHumidity=%.2f insideTemperature=%.2f\n",
                  outsideTemperature, outsideHumidity, insideTemperature);

    if (!isWifiConnected()) {
        connectToWifi();
    }

    WiFiClient client;
    HTTPClient http;

    http.begin(client, API_HOST + "/temperature");

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    snprintf(httpRequestBody, sizeof(httpRequestBody), "key=%s&temperature=%.2f&humidity=%.2f&temp2=%.2f", API_KEY,
             outsideTemperature, outsideHumidity, insideTemperature);

    http.POST(httpRequestBody);

    http.end();

    digitalWrite(LED_BUILTIN, HIGH);
}

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

    Serial.println("Initialized");

    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
    handleArduinoOta();

    updateTemperature();
    delay(3000);
}
