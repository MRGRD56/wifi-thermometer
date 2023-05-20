#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_HTU21DF.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "ESP8266WebServer.h"
#include <U8g2lib.h>
#include <EEPROM.h>
#include "../lib/Credentials.h"
#include "../lib/ota/Ota.h"
#include "../lib/thermistor_xreef/Thermistor.h"
#include "../lib/temperature/Temperature.h"
#include "../lib/button/Button.h"

#define EEPROM_SIZE 1
#define EEPROM_ADDR_IS_DATA_SHOWN 0

const String API_KEY = "a9b43ee71309";
const String API_HOST = "http://192.168.0.132:8080";

const char* COLLECTED_HEADERS[] = {"X-Api-Key"};
const size_t COLLECTED_HEADERS_SIZE = sizeof(COLLECTED_HEADERS) / sizeof(char*);

ESP8266WebServer server = ESP8266WebServer(80);

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2 = U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R2, U8X8_PIN_NONE, SCL, SDA);
Adafruit_HTU21DF htu = Adafruit_HTU21DF();
Thermistor thermistor = Thermistor(A0, false);
Button button = Button(D6);

unsigned long lastDataUpdate = 0;

bool isEcoMode;

void setLedLight(boolean isEnabled) {
    if (isEnabled) {
        digitalWrite(LED_BUILTIN, LOW);
    } else {
        digitalWrite(LED_BUILTIN, HIGH);
    }
}

void clearDisplay() {
    u8g2.clearDisplay();
}

void displayData(TemperatureData data) {
    if (isEcoMode) {
        clearDisplay();
        return;
    }

    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_10x20_t_cyrillic);
        u8g2.setCursor(0, 16);
        u8g2.printf("To: %.2f *C", data.outside.temperature);
        u8g2.setCursor(0, 38);
        u8g2.printf("Ho: %.2f %%", data.outside.humidity);
        u8g2.setCursor(0, 60);
        u8g2.printf("Ti: %.2f *C", data.inside.temperature);
    } while (u8g2.nextPage());
}

void displayText(const char *format...) {
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_10x20_t_cyrillic);
        u8g2.setCursor(0, 38);
        u8g2.printf(format);
    } while (u8g2.nextPage());
}

void setEcoMode(bool newEcoMode) {
    isEcoMode = newEcoMode;
        
    EEPROM.write(EEPROM_ADDR_IS_DATA_SHOWN, isEcoMode);
    EEPROM.commit();

    if (isEcoMode) {
        clearDisplay();
    } else {
        lastDataUpdate = 0;
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

    displayText("Connecting");

    int i = 0;
    while (!isWifiConnected()) {
        setLedLight(false);
        delay(70);
        setLedLight(true);
        delay(1000 - 70);

        i++;

        Serial.print(i);
        Serial.print(' ');

        switch (i % 3) {
            case 0:
                displayText("Connecting.");
                break;
            case 1:
                displayText("Connecting..");
                break;
            case 2:
                displayText("Connecting...");
                break;
            default:
                break;
        }
    }

    Serial.println('\n');
    Serial.println("Connection established!");
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());

    displayText("Connected");

    setLedLight(false);
}

TemperatureData getTemperature() {
    setLedLight(true);

    float outsideTemperature = htu.readTemperature();
    float outsideHumidity = htu.readHumidity();
    float insideTemperature = thermistor.readTemperature();

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

auto authorized(void (*handler)()) {
    return [handler]() {
        Serial.println("Headers:");
        for (uint8_t i = 0; i < server.headers(); i++) {
            Serial.println(server.headerName(i) + ": " + server.header(i));
        }

        String apiKey = server.header("X-Api-Key");

        if (API_KEY != nullptr && API_KEY != apiKey) {
            Serial.println("GET /data: 403 Forbidden - invalid api key provided");

            server.send(403, "text/plain", "Invalid api key provided");
            return;
        }

        handler();
    };
}

char getTemperatureResponseBody[128];
void handleGetTemperature() {
    TemperatureData data = getTemperature();

    snprintf(getTemperatureResponseBody, sizeof(getTemperatureResponseBody),
             R"({"outside":{"temperature":%.2f,"humidity":%.2f},"inside":{"temperature":%.2f}})",
             data.outside.temperature,
             data.outside.humidity,
             data.inside.temperature);

    server.send(200, "application/json", getTemperatureResponseBody);

    Serial.print("GET /temperature: 200 OK - ");
    Serial.println(getTemperatureResponseBody);
}

char getEcoResponseBody[32];

void handleGetEcoInternal() {
    snprintf(getEcoResponseBody, sizeof(getEcoResponseBody),
             R"({"isEco":%s})",
             isEcoMode ? "true" : "false");

    server.send(200, "application/json", getEcoResponseBody);
}

void handleGetEco() {
    handleGetEcoInternal();

    Serial.print("GET /eco: 200 OK - ");
    Serial.println(getEcoResponseBody);
}

void handlePostEco() {
    String newValue = server.arg("isEco");
    if (newValue == "true") {
        setEcoMode(true);
    } else if (newValue == "false") {
        setEcoMode(false);
    } else {
        server.send(400, "text/plain", "isEco boolean parameter is required");
        return;
    }

    handleGetEcoInternal();

    Serial.print("POST /eco: 200 OK - ");
    Serial.println(getEcoResponseBody);
}

//char httpRequestBody[128];

//void updateTemperature() {
//    setLedLight(true);
//
//    float temperature = htu.readTemperature();
//    float humidity = htu.readHumidity();
//    float insideTemperature = thermistor_mg56.getTemperatureAvg();
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

void initializeThermistor() {
    thermistor.setSeriesResistor(10'000);
    thermistor.setThermistorNominalResistance(10'000); // If nothing passed set to default 10000
    thermistor.setBCoefficent(3950); // If nothing passed set to default 3950
    thermistor.setTemperatureNominal(25);
    thermistor.setVccToThermisor(true); // default Vcc to thermistor
}

void initializeServer() {
    server.collectHeaders(COLLECTED_HEADERS, COLLECTED_HEADERS_SIZE);

    server.on("/temperature", HTTP_GET, authorized(handleGetTemperature));
    server.on("/eco", HTTP_GET, authorized(handleGetEco));
    server.on("/eco", HTTP_POST, authorized(handlePostEco));
    server.enableCORS(true);
    server.begin();
}

void displayInitialData() {
    if (isEcoMode) {
        displayText("ECO mode");
        delay(500);
        clearDisplay();
    } else {
        displayData(getTemperature());
    }
}

void setup() {
    initializeArduinoOta();

    EEPROM.begin(EEPROM_SIZE);
    isEcoMode = EEPROM.read(EEPROM_ADDR_IS_DATA_SHOWN);

    u8g2.begin();

    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(9600);

    initializeThermistor();
    button.initialize();

    if (!htu.begin()) {
        Serial.println("Check circuit. HTU21D not found!");
        displayText("HTU21D not found!");
        delay(1000);
        EspClass::restart();
    }

    connectToWifi();

    initializeServer();

    Serial.println("Initialized");

    setLedLight(true);
    delay(120);
    setLedLight(false);
    delay(80);
    setLedLight(true);
    delay(120);
    setLedLight(false);

    displayInitialData();
}

void loop() {
    if (button.isPressed()) {
        setEcoMode(!isEcoMode);
    }

    handleArduinoOta();
    server.handleClient();

    unsigned long now = millis();

    if (!isEcoMode) {
        if (lastDataUpdate < now - 1000) {
            lastDataUpdate = now;
            displayData(getTemperature());
        }
    }
}
