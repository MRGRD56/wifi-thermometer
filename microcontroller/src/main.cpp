#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_HTU21DF.h"
#include <EEPROM.h>

#include "../lib/Credentials.h"
#include "../lib/ota/Ota.h"
#include "../lib/thermistor_xreef/Thermistor.h"
#include "../lib/temperature/Temperature.h"
#include "../lib/button/Button.h"

#ifdef ESP8266
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "ESP8266WebServer.h"
#include "../lib/presence/PresenceSensor.h"

#else
#include "WiFi.h"
#include "HTTPClient.h"
#include "WebServer.h"
#endif

#define DISPLAY_LCD
//#define DISPLAY_OLED

#ifdef DISPLAY_OLED
#include <U8g2lib.h>
#endif

#ifdef DISPLAY_LCD
#include "LiquidCrystal_I2C.h"
#include "../lib/lcd/LcdCustomCharacters.h"
#endif

#define EEPROM_SIZE 1
#define EEPROM_ADDR_IS_DATA_SHOWN 0

const unsigned long PRESENCE_MIN_DURATION = (8 * 60 * 1000);

const String API_KEY = "a9b43ee71309";
const String API_HOST = "http://192.168.0.132:8080";

const char* COLLECTED_HEADERS[] = {"X-Api-Key"};
const size_t COLLECTED_HEADERS_SIZE = sizeof(COLLECTED_HEADERS) / sizeof(char*);

ESP8266WebServer server = ESP8266WebServer(80);

#ifdef DISPLAY_OLED
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2 = U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R2, U8X8_PIN_NONE, SCL, SDA);
#endif
#ifdef DISPLAY_LCD
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);
#endif
Adafruit_HTU21DF htu = Adafruit_HTU21DF();
Thermistor thermistor = Thermistor(A0, false);
Button button = Button(D6);
PresenceSensor presenceSensor = PresenceSensor(D5);

unsigned long lastDataUpdate = 0;
unsigned long dataUpdateDelay;
bool lastPresent = true;

bool isEcoMode;

bool isScreenEnabled;

bool isDebugMode = false;
byte debugPageIndex = 0;
const byte debugPagesCount = 2;
unsigned long lastDebugUpdate = 0;

void setLedLight(boolean isEnabled) {
    if (isEnabled) {
        digitalWrite(LED_BUILTIN, LOW);
    } else {
        digitalWrite(LED_BUILTIN, HIGH);
    }
}

void initDisplay() {
    isScreenEnabled = true;

#ifdef DISPLAY_OLED
    u8g2.begin();
#endif

#ifdef DISPLAY_LCD
    lcd.init();
    lcd.backlight();

    lcd.createChar(LCD_CHAR_CELSIUS_DEGREES, LCD_CHAR_CELSIUS_DEGREES_BYTES);
    lcd.createChar(LCD_CHAR_INSIDE, LCD_CHAR_INSIDE_BYTES);
    lcd.createChar(LCD_CHAR_OUTSIDE, LCD_CHAR_OUTSIDE_BYTES);
#endif
}

void disableDisplay() {
    if (!isScreenEnabled) {
        return;
    }

#ifdef DISPLAY_OLED
    u8g2.clearDisplay();
#endif

#ifdef DISPLAY_LCD
    lcd.noBacklight();
    lcd.noDisplay();
#endif

    isScreenEnabled = false;
}

void displayData(TemperatureData data) {
    if (isEcoMode) {
        disableDisplay();
        return;
    }

#ifdef DISPLAY_OLED
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
#endif

#ifdef DISPLAY_LCD
    lcd.clear();
    if (!isScreenEnabled) {
        lcd.backlight();
        lcd.display();
    }
    lcd.setCursor(0, 0);
    lcd.write(LCD_CHAR_OUTSIDE);
    lcd.printf(" %.2f", data.outside.temperature);
    lcd.write(LCD_CHAR_CELSIUS_DEGREES);
    lcd.printf(" %.1f%%", data.outside.humidity);

    lcd.setCursor(0, 1);
    lcd.write(LCD_CHAR_INSIDE);
    lcd.printf(" %.2f", data.inside.temperature);
    lcd.write(LCD_CHAR_CELSIUS_DEGREES);
#endif

    isScreenEnabled = true;
}

void displayText(const char *format, ...) {
    va_list args;
    va_start(args, format);

    int bufferSize = vsnprintf(nullptr, 0, format, args) + 1;
    char* buffer = new char[bufferSize];

    vsnprintf(buffer, bufferSize, format, args);

#ifdef DISPLAY_OLED
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_10x20_t_cyrillic);
        u8g2.setCursor(0, 38);
        u8g2.printf("%s", buffer);
    } while (u8g2.nextPage());
#endif

#ifdef DISPLAY_LCD
    lcd.clear();
    if (!isScreenEnabled) {
        lcd.backlight();
        lcd.display();
    }
    lcd.setCursor(0, 0);
    lcd.printf("%s", buffer);
#endif

    delete[] buffer;
    va_end(args);

    isScreenEnabled = true;
}

void displayEcoIntro() {
    displayText("ECO mode");
    delay(500);
}

void setEcoMode(bool newEcoMode) {
    isEcoMode = newEcoMode;
        
    EEPROM.write(EEPROM_ADDR_IS_DATA_SHOWN, isEcoMode);
    EEPROM.commit();

    if (isEcoMode) {
        disableDisplay();
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
        delay(1000);

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
    float outsideTemperature = htu.readTemperature();
    float outsideHumidity = htu.readHumidity();
    float insideTemperature = thermistor.readTemperature();

    return {
        {
            outsideTemperature,
            outsideHumidity
        },
        {
            insideTemperature,
            -1
        }
    };
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

char getPresenceBody[64];

void handleGetPresence() {
    bool* isPresentCurrentlyPtr = new bool;
    bool isPresent = presenceSensor.isPresentLong(PRESENCE_MIN_DURATION, isPresentCurrentlyPtr);

    snprintf(getPresenceBody, sizeof(getPresenceBody),
             R"({"isPresent":%s,"isPresentNow":%s})",
             isPresent ? "true" : "false",
             *isPresentCurrentlyPtr ? "true" : "false");

    delete isPresentCurrentlyPtr;

    server.send(200, "application/json", getPresenceBody);

    Serial.print("GET /presence: 200 OK - ");
    Serial.println(getPresenceBody);
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
    server.on("/presence", HTTP_GET, authorized(handleGetPresence));
    server.enableCORS(true);
    server.begin();
}

void displayInitialData() {
    if (isEcoMode) {
        displayEcoIntro();
        disableDisplay();
    } else {
        displayData(getTemperature());
    }
}

void setDebugPage(byte page) {
    debugPageIndex = page % debugPagesCount;
    lastDebugUpdate = 0;
    if (debugPageIndex == 1) {
        dataUpdateDelay = 300;
    } else {
        dataUpdateDelay = 5000;
    }
}

void displayDebugPage() {
    switch (debugPageIndex) {
        case 0:
            displayText(WiFi.localIP().toString().c_str());
            break;
        case 1:
            displayText("Pres: %s", presenceSensor.isPresentNow() ? "Y" : "N");
            break;
        default:
            break;
    }
}

void setup() {
    initializeArduinoOta();

    EEPROM.begin(EEPROM_SIZE);
    isEcoMode = EEPROM.read(EEPROM_ADDR_IS_DATA_SHOWN);

    initDisplay();

    pinMode(LED_BUILTIN, OUTPUT);

    presenceSensor.initialize();
    presenceSensor.setPresent(true);

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
    unsigned long buttonPressMillis;
    if (button.isPressed(buttonPressMillis, 1500)) {
        presenceSensor.setPresent(true);

        if (buttonPressMillis >= 1500) {
            if (!isEcoMode || isDebugMode) {
                isDebugMode = !isDebugMode;

                if (isDebugMode) {
                    displayText("Debug ON");
                    delay(500);
                    setDebugPage(0);
                } else {
                    displayText("Debug OFF");
                    delay(500);
                    lastDataUpdate = 0;
                }
            }
        } else if (buttonPressMillis >= 80) {
            if (isDebugMode) {
                setDebugPage(debugPageIndex + 1);
            } else {
                if (isEcoMode || lastPresent) {
                    bool newEcoMode = !isEcoMode;

                    if (newEcoMode) {
                        displayEcoIntro();
                    }

                    setEcoMode(newEcoMode);
                } else {
                    lastDataUpdate = 0;
                }
            }
        }
    }

    handleArduinoOta();
    server.handleClient();

    unsigned long now = millis();

    if (isDebugMode) {
        if (lastDebugUpdate < now - dataUpdateDelay) {
            lastDebugUpdate = now;

            displayDebugPage();
        }
    } else {
        if (!isEcoMode) {
            if (lastDataUpdate < now - 3000) {
                lastDataUpdate = now;

                bool previousPresent = lastPresent;
                bool isPresent = presenceSensor.isPresentLong(PRESENCE_MIN_DURATION);
                lastPresent = isPresent;

                if (isPresent) {
                    displayData(getTemperature());
                } else if (previousPresent) {
                    disableDisplay();
                }
            }
        }
    }

    if (presenceSensor.getLastUpdate() < now - 7'000) {
        presenceSensor.isPresentLong(PRESENCE_MIN_DURATION);
    }
}
