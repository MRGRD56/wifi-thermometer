#include <Arduino.h>
#include "Adafruit_HTU21DF.h"
#include "Wire.h"
#include "I2C_eeprom.h"
//#include "OneWire.h"
#include <ArduinoHA.h>

#include "../lib/Credentials.h"
//#include "../lib/config/Config.h"
//#include "../lib/config/WiFiCredentials.h"
#include "../lib/ota/Ota.h"
#include "Weather.h"
#include "../lib/button/Button.h"
#include "../lib/presence/PresenceSensor.h"
#include "../lib/weather/OnlineWeather.h"


#define PIN_BUTTON 18
#define PIN_PRESENCE_SENSOR 27

#ifdef ESP8266
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "ESP8266WebServer.h"

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

#define BROKER_ADDR     IPAddress(192, 168, 0, 100)

//#define LED_RECEIVE 0
//#define LED_RECEIVE_ON 1

const unsigned long PRESENCE_MIN_DURATION = (3 * 60 * 1000);

const String API_KEY = "a9b43ee71309";

const char* COLLECTED_HEADERS[] = {"X-Api-Key"};
const size_t COLLECTED_HEADERS_SIZE = sizeof(COLLECTED_HEADERS) / sizeof(char*);

byte mac[] = {0x90, 0xDA, 0x9B, 0x1B, 0x5E, 0xAE};

WiFiClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);

HASensorNumber haOutsideTemperatureSensor("outside_temperature", HASensorNumber::PrecisionP2);
HASensorNumber haOutsideHumiditySensor("outside_humidity", HASensorNumber::PrecisionP2);
HASensorNumber haInsideTemperatureSensor("inside_temperature", HASensorNumber::PrecisionP2);
HASensorNumber haInsideHumiditySensor("inside_humidity", HASensorNumber::PrecisionP2);
HASwitch haDisplaySwitch("esp_thermometer_display_switch");

WebServer server(80);

I2C_eeprom eeprom(0x50, I2C_DEVICESIZE_24LC256);

#ifdef DISPLAY_OLED
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2 = U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R2, U8X8_PIN_NONE, SCL, SDA);
#endif
#ifdef DISPLAY_LCD
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);
#endif
Adafruit_HTU21DF htu = Adafruit_HTU21DF();
OnlineWeather onlineWeather(WEATHER_API_KEY);
Button button(PIN_BUTTON);
PresenceSensor presenceSensor(PIN_PRESENCE_SENSOR);

unsigned long lastOnlineWeatherUpdate = 0;

unsigned long lastDataUpdate = 0;
unsigned long dataUpdateDelay;
bool lastPresent = true;

bool isEcoMode;

bool isScreenEnabled;

bool isDebugMode = false;
byte debugPageIndex = 0;
const byte debugPagesCount = 2;
unsigned long lastDebugUpdate = 0;

unsigned long buttonPressMillis = 0;

void createTask(const char* name, void (*taskFunc)(), uint16_t stackSize = 2048, UBaseType_t priority = 1) {
    xTaskCreate(
            [](void* parameter) {
                // Вызов переданной функции
                ((void (*)())parameter)();
                // Удаление задачи после завершения выполнения функции
                vTaskDelete(NULL);
            },
            name,
            stackSize,
            (void*)taskFunc,
            priority,
            nullptr
    );
}

void createTask(void (*taskFunc)(), uint16_t stackSize = 2048, UBaseType_t priority = 1) {
    createTask(nullptr, taskFunc, stackSize, priority);
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
    lcd.createChar(LCD_CHAR_OUTSIDE_CLEAR, LCD_CHAR_OUTSIDE_CLEAR_BYTES);
    lcd.createChar(LCD_CHAR_OUTSIDE_FEW_CLOUDS, LCD_CHAR_OUTSIDE_FEW_CLOUDS_BYTES);
    lcd.createChar(LCD_CHAR_OUTSIDE_CLOUDY, LCD_CHAR_OUTSIDE_CLOUDY_BYTES);
    lcd.createChar(LCD_CHAR_OUTSIDE_PRECIPITATION, LCD_CHAR_OUTSIDE_PRECIPITATION_BYTES);
    lcd.createChar(LCD_CHAR_OUTSIDE_THUNDERSTORM, LCD_CHAR_OUTSIDE_THUNDERSTORM_BYTES);
    lcd.createChar(LCD_CHAR_OUTSIDE_MIST, LCD_CHAR_OUTSIDE_MIST_BYTES);
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

byte convertOutsideCharacter(WeatherType weatherType) {
    switch (weatherType) {
        case THUNDERSTORM:
            return LCD_CHAR_OUTSIDE_THUNDERSTORM;
        case RAIN:
        case DRIZZLE:
        case SNOW:
            return LCD_CHAR_OUTSIDE_PRECIPITATION;
        case ATMOSPHERE:
            return LCD_CHAR_OUTSIDE_MIST;
        case CLEAR:
            return LCD_CHAR_OUTSIDE_CLEAR;
        case FEW_CLOUDS:
            return LCD_CHAR_OUTSIDE_FEW_CLOUDS;
        case CLOUDS:
            return LCD_CHAR_OUTSIDE_CLOUDY;
        default:
            return LCD_CHAR_OUTSIDE_CLEAR;
    }
}

void displayWeather(Weather data) {
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
    if (!isScreenEnabled) {
        lcd.backlight();
        lcd.display();
    }

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.write(convertOutsideCharacter(data.outside.weatherType));
    lcd.printf(" %.2f", data.outside.temperature);
    lcd.write(LCD_CHAR_CELSIUS_DEGREES);
    lcd.printf(" %.1f%%", data.outside.humidity);

    lcd.setCursor(0, 1);
    lcd.write(LCD_CHAR_INSIDE);
    lcd.printf(" %.2f", data.inside.temperature);
    lcd.write(LCD_CHAR_CELSIUS_DEGREES);
    lcd.printf(" %.1f%%", data.inside.humidity);
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
        u8g2.print(buffer);
    } while (u8g2.nextPage());
#endif

#ifdef DISPLAY_LCD
    lcd.clear();
    if (!isScreenEnabled) {
        lcd.backlight();
        lcd.display();
    }
    lcd.setCursor(0, 0);
    lcd.print(buffer);
#endif

    delete[] buffer;
    va_end(args);

    isScreenEnabled = true;
}

void displayEcoIntro() {
    displayText("ECO mode");
    vTaskDelay(pdMS_TO_TICKS(500));
}

void setEcoMode(bool newEcoMode) {
    isEcoMode = newEcoMode;

    haDisplaySwitch.setState(!newEcoMode);
    eeprom.updateByte(EEPROM_ADDR_IS_DATA_SHOWN, isEcoMode);

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
    WiFi.mode(WIFI_MODE_STA);
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
}

Weather getTemperature() {
    if (millis() - lastOnlineWeatherUpdate > 60'000) {
        if (onlineWeather.fetchWeather(LOCATION_LATITUDE, LOCATION_LONGITUDE)) {
            lastOnlineWeatherUpdate = millis();
        }
    }

    return {
        onlineWeather.getWeather(),
        {
            htu.readTemperature(),
            htu.readHumidity()
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

//void handleGetIndex() {
//    if (!server.authenticate("user", "password")) {
//        server.requestAuthentication(BASIC_AUTH);
//        return;
//    }
//
//    server.send(200, );
//}

char getTemperatureResponseBody[128];
void handleGetTemperature() {
    Weather data = getTemperature();

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

void initializeServer() {
    server.collectHeaders(COLLECTED_HEADERS, COLLECTED_HEADERS_SIZE);

//    server.on("/", HTTP_GET, handleGetIndex);
    server.on("/temperature", HTTP_GET, authorized(handleGetTemperature));
    server.on("/eco", HTTP_GET, authorized(handleGetEco));
    server.on("/eco", HTTP_POST, authorized(handlePostEco));
    server.on("/presence", HTTP_GET, authorized(handleGetPresence));
    server.enableCORS(true);
    server.begin();
}

void displayInitialData() {
    if (isEcoMode) {
        createTask([]() {
            displayEcoIntro();
            disableDisplay();
        });
    } else {
        lastDataUpdate = millis();
        if (onlineWeather.fetchWeather(LOCATION_LATITUDE, LOCATION_LONGITUDE)) {
            lastOnlineWeatherUpdate = millis();
        }
        displayWeather(getTemperature());
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

void initializeHomeAssistant() {
    device.setName("ESP Thermometer");
    device.setSoftwareVersion("1.0.0");

    haInsideTemperatureSensor.setIcon("mdi:home-thermometer");
    haInsideTemperatureSensor.setName("Inside Temperature");
    haInsideTemperatureSensor.setUnitOfMeasurement("°C");

    haInsideHumiditySensor.setIcon("mdi:water-percent");
    haInsideHumiditySensor.setName("Inside Humidity");
    haInsideHumiditySensor.setUnitOfMeasurement("%");

    haOutsideTemperatureSensor.setIcon("mdi:sun-thermometer");
    haOutsideTemperatureSensor.setName("Outside Temperature");
    haOutsideTemperatureSensor.setUnitOfMeasurement("°C");

    haOutsideHumiditySensor.setIcon("mdi:cloud-percent");
    haOutsideHumiditySensor.setName("Outside Humidity");
    haOutsideHumiditySensor.setUnitOfMeasurement("%");

    haDisplaySwitch.setIcon("mdi:white-balance-sunny");
    haDisplaySwitch.setName("Display");
    haDisplaySwitch.onCommand([](bool isDisplayOn, HASwitch* haSwitch) {
        setEcoMode(!isDisplayOn);
    });

    mqtt.begin(BROKER_ADDR);

    mqtt.loop();

    haDisplaySwitch.setState(!isEcoMode);
}

void setup() {
    Serial.begin(9600);

    initDisplay();

    Wire.begin();

    if (!eeprom.begin()) {
        Serial.println("Check circuit. EEPROM not found!");
        displayText("EEPROM not found!");
        delay(1000);
        ESP.restart();
    }
    isEcoMode = eeprom.readByte(EEPROM_ADDR_IS_DATA_SHOWN);

//    pinMode(LED_BUILTIN, OUTPUT);

    presenceSensor.initialize();
    presenceSensor.setPresent(true);

    button.initialize();

    if (!htu.begin()) {
        Serial.println("Check circuit. HTU21D not found!");
        displayText("HTU21D not found!");
        delay(1000);
        ESP.restart();
    }

    connectToWifi();

    initializeArduinoOta();

    initializeHomeAssistant();

    initializeServer();

    displayInitialData();

    Serial.println("Initialized.");

    createTask([]() {
        while (true) {
            button.isPressed(buttonPressMillis, 1500);
            vTaskDelay(1);
        }
    });

    createTask([]() {
        while (true) {
            handleArduinoOta();
            vTaskDelay(pdMS_TO_TICKS(350));
        }
    });

    createTask([]() {
        while (true) {
            server.handleClient();
            vTaskDelay(1);
        }
    });

    createTask([]() {
        while (true) {
            mqtt.loop();
            vTaskDelay(1);
        }
    });

    createTask([]() {
        while (true) {
            // if Wi-Fi is down, try reconnecting
            if (!isWifiConnected()) {
                Serial.print(millis());
                Serial.println("Reconnecting to WiFi...");
                WiFi.disconnect();
                WiFi.reconnect();
            }

            vTaskDelay(pdMS_TO_TICKS(30'000));
        }
    });
}

void loop() {
    if (buttonPressMillis) {
        ulong pressMillis = buttonPressMillis;
        buttonPressMillis = 0;

        presenceSensor.setPresent(true);

        if (!isEcoMode && pressMillis >= 1500) {
            isDebugMode = !isDebugMode;

            if (isDebugMode) {
                displayText("Debug ON");
                vTaskDelay(pdMS_TO_TICKS(500));
                setDebugPage(0);
            } else {
                displayText("Debug OFF");
                vTaskDelay(pdMS_TO_TICKS(500));
                lastDataUpdate = 0;
            }
        } else if (pressMillis >= 25) {
            if (isDebugMode) {
                setDebugPage(debugPageIndex + 1);
            } else {
                if (isEcoMode || lastPresent) {
                    bool newEcoMode = !isEcoMode;

                    setEcoMode(newEcoMode);
                    if (newEcoMode) {
                        displayEcoIntro();
                        disableDisplay();
                    }
                } else {
                    lastDataUpdate = 0;
                }
            }
        }
    }

    unsigned long now = millis();

    bool isDataUpdate = lastDataUpdate < now - 3000;
    Weather temperatureData;
    if (isDataUpdate) {
        temperatureData = getTemperature();

        haInsideTemperatureSensor.setValue(temperatureData.inside.temperature);
        haInsideHumiditySensor.setValue(temperatureData.inside.humidity);
        haOutsideTemperatureSensor.setValue(temperatureData.outside.temperature);
        haOutsideHumiditySensor.setValue(temperatureData.outside.humidity);

        lastDataUpdate = now;
    }

    if (isDebugMode) {
        if (lastDebugUpdate < now - dataUpdateDelay) {
            lastDebugUpdate = now;

            displayDebugPage();
        }
    } else {
        if (!isEcoMode) {
            if (isDataUpdate) {
                bool previousPresent = lastPresent;
                bool isPresent = presenceSensor.isPresentLong(PRESENCE_MIN_DURATION);
                lastPresent = isPresent;

                if (isPresent) {
                    displayWeather(temperatureData);
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
