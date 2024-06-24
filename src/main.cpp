#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1305.h>
#include <SPI.h>
#include <TimeLib.h>
#include <Wire.h>

#include "nvs_flash.h"

#include <fall_detection.h>
#include <log.h>
#include <math.h>
#include <pulse_oximeter.h>
#include <system_time.h>
#include <wifi.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define BUTTON_0 17
#define BUTTON_1 18
#define BUTTON_2 21
#define BUTTON_3 48
#define OLED_RESET -1

Pulse_Oximeter sensor{};
Adafruit_SSD1305 tft(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int state = 0;
volatile bool firstButtonPressed = false;
volatile bool secondButtonPressed = false;
volatile bool thirdButtonPressed = false;
volatile bool fourthButtonPressed = false;

void IRAM_ATTR handleFirstButtonPress() {
    firstButtonPressed = true;
}
void IRAM_ATTR handleSecondButtonPress() {
    secondButtonPressed = true;
}
void IRAM_ATTR handleThirdButtonPress() {
    thirdButtonPressed = true;
}
void IRAM_ATTR handleFourthButtonPress() {
    fourthButtonPressed = true;
}

// init setup for the time keeping
uint32_t targetTime = 0;
byte omm = 99;
static uint8_t conv2d(const char* p) {
    uint8_t v = 0;
    if ('0' <= *p && *p <= '9')
        v = *p - '0';
    return 10 * v + *++p - '0';
}
uint8_t hh=conv2d(__TIME__), mm=conv2d(__TIME__+3), ss=conv2d(__TIME__+6);

void updateBattery();
void updateHeartrate();
void updateTime();

void setup() {
    Serial.begin(9600);
    Wire.setPins(15, 16);

    // flash memory must be initialized before enabling Wi-Fi
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // initialize Wi-Fi in station mode
    if (!wifi::init_sta(wifi::ssid, wifi::password)) {
        Serial.println("ERROR: Failed to connect to WiFi. Check your credentials.");
    } else {
        Serial.println("Successfully established a WiFi connection.");
    }

    // log in to web server
    if (!pi::login("admin", "admin")) {
        Serial.println("ERROR: Failed to connect to Web server.");
    } else {
        Serial.println("Successfully connected to the web server.");
    }

    pi::log("hello world");
    pi::log_heart_rate(90);
    system_time::sync();
    fall_detection::start();
    sensor.start();

    pinMode(BUTTON_0, INPUT);
    pinMode(BUTTON_1, INPUT);
    pinMode(BUTTON_2, INPUT);
    pinMode(BUTTON_3, INPUT);

    attachInterrupt(digitalPinToInterrupt(BUTTON_0), handleFirstButtonPress, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_1), handleSecondButtonPress, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_2), handleThirdButtonPress, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_3), handleFourthButtonPress, FALLING);

    tft.begin(SSD1305_I2C_ADDRESS, OLED_RESET); // init
    tft.clearDisplay();

    targetTime = millis() + 1000;

    sensor.start();
}

void loop() {
    tft.clearDisplay();

    // change state so it shows the time
    if (digitalRead(BUTTON_0) == HIGH) {
        state = 0;
    }

    // change state so it shows the heartrate
    if (digitalRead(BUTTON_1) == HIGH) {
        state = 1;
    }

    // change state so it shows the battery percentage
    if (digitalRead(BUTTON_2) == HIGH) {
        state = 2;
    }

    if ((state % 3) == 0) {
        updateTime();
    } else if ((state % 3) == 1) {
        updateHeartrate();
    } else if ((state % 3) == 2) {
        updateBattery();
    }

    delay(1000);
}

void updateTime() {
    if (targetTime < millis()) {
        targetTime = millis()+1000;
        ss++;              // Advance second
        if (ss==60) {
            ss=0;
            omm = mm;
            mm++;            // Advance minute
            if(mm>59) {
                mm=0;
                hh++;          // Advance hour
                if (hh>23) {
                    hh=0;
                }
            }
        }
    }

    const auto now = system_time::now();
    hh = now.hour;
    mm = now.minute;
    ss = now.second;

    tft.setCursor(15, 9);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.print(hh);
    tft.print(":");
    if (mm < 10) {
        tft.print("0");
    }
    tft.print(mm);
    tft.print(":");
    if (ss < 10) {
        tft.print("0");
    }
    tft.print(ss);
    tft.display();
}

void updateHeartrate() {
    sensor.measure_bpm();
    tft.setCursor(25, 9);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.print(sensor.bpm_value());
    tft.setCursor(80, 9);
    tft.print("bpm");
    tft.display();


    static auto prev = system_time::now();
    const auto now = system_time::now();
    // log every 5 mins
    if (now.minute - prev.minute > 5) {
        pi::log_heart_rate(sensor.bpm_value());
        prev = now;
    }
}

void updateBattery() {
    tft.setCursor(10, 5);
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.print("Battery percentage");

    // outline
    tft.fillRect(8, 18, 112, 10, WHITE);
    tft.fillRect(9, 19, 110, 8, BLACK);

    // battery percentage
    tft.fillRect(10, 20, 92, 6, WHITE);

    tft.display();
}
