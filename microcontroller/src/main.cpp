#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_HTU21DF.h"

Adafruit_HTU21DF htu = Adafruit_HTU21DF();

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
  float temp = htu.readTemperature();
  float hum = htu.readHumidity();
  Serial.print("Temperature(°C): "); 
  Serial.print(temp); 
  Serial.print("\t\t");
  Serial.print("Humidity(%): "); 
  Serial.println(hum);
  delay(500);
}
