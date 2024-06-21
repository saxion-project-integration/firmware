# Firmware

Contains firmware for the ESP32 microcontroller.

## Build Prerequisites

* Before building, make sure that `CONFIG_FREERTOS_HZ` is set to `1000` and that `CONFIG_AUTOSTART_ARDUINO` is set to `y` in `sdkconfig.*`.
* In `include/wifi.h`, make sure that `wifi::ssid` and `wifi::password` are set to the credentials of the WiFi network you want to connect to.
* In `src/log.cpp`, make sure that `pi::web_server_ip` is set to the IP address of the web server.
Because the web server is not deployed yet, use a placeholder value to ingore the error.
