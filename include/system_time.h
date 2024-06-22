/**
 * @file system_time.h
 * @brief Contains system time related function(s).
 */

#ifndef SYSTEM_TIME_H
#define SYSTEM_TIME_H

#include <Arduino.h>

#include "esp_log.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"

#include <http.h>

namespace system_time {
    /**
     * Represents a point in time within a day.
     */
    struct timestamp {
        int hour;
        int minute;
        int second;
    };

    /**
     * Synchronizes the local system's time to UTC+2.
     *
     * @note A WiFi connection should be established before calling this function.
     */
    inline void sync() {
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

    /**
     * Get the current system time.
     *
     * @return The current system time.
     */
    inline timestamp now() {
        time_t now;
        time(&now);
        struct tm* tm = localtime(&now);

        return timestamp{
            .hour = tm->tm_hour,
            .minute = tm->tm_min,
            .second = tm->tm_sec,
        };
    }
}

#endif
