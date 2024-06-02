/**
 * @file wifi.cpp
 * @brief Contains function(s) for establishing a Wi-Fi connection.
 */

#include <algorithm>
#include <cstdint>

#include <Arduino.h>
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"

#include <wifi.h>

namespace wifi { 
    namespace {
        /**
         * Event group for Wi-Fi and IP related events.
         */
        EventGroupHandle_t event_group{};

        /**
         * A bit for the event group indicating that a Wi-Fi connection was successfully established.
         */
        constexpr EventBits_t connected_bit = (1 << 0);

        /**
         * A bit for the event group indicating that a Wi-Fi connection failed to establish.
         */
        constexpr EventBits_t fail_bit = (1 << 1);

        /**
         * Event handler for Wi-Fi and IP events.
         *
         * This function handles Wi-Fi and IP events such as receiving an IPV4 address from a DHCP server,
         * a Wi-Fi disconnection, and starting the WiFi module.
         */
        void event_handler(void* arg, esp_event_base_t event_base, std::int32_t event_id, void* event_data) {
            constexpr std::size_t max_retry_num = 0uz;
            static std::size_t retry_num = 0uz;

            if (event_base == WIFI_EVENT) {
                if (event_id == WIFI_EVENT_STA_START) {
                    esp_wifi_connect();
                } 
                else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
                    // retry to connect a couple of times
                    if (retry_num < max_retry_num) {
                        esp_wifi_connect();
                        ++retry_num;
                    } 
                    else {
                        xEventGroupSetBits(event_group, fail_bit);
                    }
                }
            }
            else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
                retry_num = 0;
                xEventGroupSetBits(event_group, connected_bit);
            }
        }

    }

    bool init_sta(const std::string& ssid, const std::string& password) {
        if (!(event_group = xEventGroupCreate())) {
            return false;
        }

        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        esp_netif_create_default_wifi_sta();

        wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&config));

        // register event handlers for Wi-Fi and IP
        esp_event_handler_instance_t any_id{};
        esp_event_handler_instance_t got_ip{};
        ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler, nullptr, &any_id));
        ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, event_handler, nullptr, &got_ip));

        wifi_config_t wifi_config{};
        
        // copy the SSID
        std::size_t n = std::ranges::size(wifi_config.sta.ssid);
        std::ranges::copy_n(ssid.begin(), n, wifi_config.sta.ssid);
        
        // copy the password
        n = std::ranges::size(wifi_config.sta.password);
        std::ranges::copy_n(password.begin(), n, wifi_config.sta.password);

        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());

        auto bits = xEventGroupWaitBits(event_group, connected_bit | fail_bit, pdTRUE, pdFALSE, portMAX_DELAY);
        return bits & connected_bit;
    }
}

