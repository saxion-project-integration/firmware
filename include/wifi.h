/**
 * @file wifi.h
 * @brief Contains function(s) for establishing a Wi-Fi connection.
 */

#ifndef WIFI_H
#define WIFI_H

#include <string>

namespace wifi {
    inline constexpr std::string ssid{""};
    inline constexpr std::string password{""};

    static_assert(!ssid.empty(), "`wifi::ssid` must be initialized with a non-empty string");
    static_assert(!password.empty(), "`wifi::password` must be be initialized with a non-empty string");

    /**
     * Initialize Wi-Fi in station mode.
     *
     * This function initializes Wi-Fi in station mode and attempts to establish a connection.
     *
     * @param ssid The SSID of the Wi-Fi network to connect with.
     * @param password The password of the Wi-Fi network.
     * @return `true` if a connection was established, `false` otherwise.
     */
    bool init_sta(const std::string& ssid, const std::string& password);
}

#endif

