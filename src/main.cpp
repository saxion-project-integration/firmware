#include <Arduino.h>

#include "nvs_flash.h"

#include <fall_detection.h>
#include <wifi.h>
#include <log.h>

void setup() {
    Serial.begin(9600);

    // flash memory must be initialized before enabling Wi-Fi
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // initialize Wi-Fi in station mode
    if (!wifi::init_sta(wifi::ssid, wifi::password)) {
        Serial.print("ERROR: failed to connect to WiFi. Check your credentials.");
    }

    // log in to web server
    if (!pi::login("admin", "admin")) {
        Serial.println("ERROR: failed to connect to Web server.");
    }

    // log a message
    pi::log("a test message");

    fall_detection::start();
}

void loop() {
    delay(500);
}
