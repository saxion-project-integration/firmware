#ifndef SYSTEM_TIME_H
#define SYSTEM_TIME_H

#include <Arduino.h>

#include "esp_log.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"

#include <http.h>

namespace system_time {
    /**
     * Sets the local system's timezone to UTC+2
     */
    void sync() {
        esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
        esp_netif_sntp_init(&config);

        ESP_ERROR_CHECK(esp_netif_sntp_sync_wait(pdMS_TO_TICKS(10000)));

        time_t now;
        char strftime_buf[64];
        struct tm timeinfo;

        time(&now);
        setenv("TZ", "UTC-2", 1);
        tzset();
    }
}

#endif
