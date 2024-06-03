#include <string_view>
#include <cstdio>

#include <Arduino.h>
#include "esp_log.h"
#include "esp_http_client.h"
#include "nvs_flash.h"

#include <wifi.h>
#include <log.h>

void setup() {
    // flash memory must be initialized before enabling Wi-Fi
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // initialize Wi-Fi in station mode
    wifi::init_sta(wifi::ssid, wifi::password);

    // log in to web server
    pi::login("admin", "admin");

    // log a message
    pi::log("a test message");
}

void loop() {

}

