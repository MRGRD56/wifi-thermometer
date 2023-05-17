#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_HTU21DF.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "../lib/Credentials.h"

const char* API_KEY = "a9b43ee71309";
const String API_HOST = "http://192.168.0.132:8080";

Adafruit_HTU21DF htu = Adafruit_HTU21DF();

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
        Serial.print(++i); Serial.print(' ');
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

    float temp = htu.readTemperature();
    float hum = htu.readHumidity();

    Serial.printf("NEW_DATA temp=%.2f hum=%.2f\n", temp, hum);

    if (!isWifiConnected()) {
        connectToWifi();
    }

    WiFiClient client;
    HTTPClient http;

    http.begin(client, API_HOST + "/temperature");

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    snprintf(httpRequestBody, sizeof(httpRequestBody), "key=%s&temperature=%.2f&humidity=%.2f", API_KEY, temp, hum);

    http.POST(httpRequestBody);

    http.end();

    digitalWrite(LED_BUILTIN, HIGH);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);

  if (!htu.begin()) {
    Serial.println("Check circuit. HTU21D not found!");
    while (1);
  }

  Serial.println("Initialized");

  digitalWrite(LED_BUILTIN, LOW);
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  updateTemperature();
  delay(3000);
}
