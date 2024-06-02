/**
 * @file log.cpp
 * @brief Contains function(s) for logging messages to the database.
 */

#include <array>
#include <cstdio>

#include <http.h>
#include <log.h>

using namespace std::literals::string_literals;
using namespace std::literals::string_view_literals;

namespace pi {
    namespace {
        /**
         * Cookie used to access resources of the web server.
         */
        std::string cookie{};

        /**
         * IP address of the web server to connect to.
         */
        const std::string web_server_ip{""};

        //static_assert(!ssid.empty(), "`pi::web_server_ip` must be initialized with a non-empty string");

        /**
         * The port where the web server is listening to.
         */
        constexpr std::uint16_t web_server_port{8080u};

        /**
         * The socket address of where the web server is listening to.
         */
        const std::string web_server_socket = web_server_ip + ":" + std::to_string(web_server_port);
    }

    bool login(const std::string& username, const std::string& password) {
        // construct the JSON
        char buffer[128];
        int len = snprintf(buffer, sizeof buffer, R"({
            "username": "%s",
            "password": "%s"
        })", username.c_str(), password.c_str());

        // only use part of the buffer that is written to
        std::string_view body{buffer, static_cast<std::size_t>(len)};

        // set `cookie` upon receiving a header
        auto http_event_handler = [](esp_http_client_event_t* event) {
            if (event->event_id == HTTP_EVENT_ON_HEADER) {
                if (event->header_key == "Set-Cookie"sv)
                    cookie = event->header_value;
            }
            return ESP_OK;
        };

        static const std::string url = "http://"s.append(web_server_socket).append("api/login");

        auto http_client = http::client_init({
            .url = url.c_str(),
            .method = HTTP_METHOD_POST,
            .event_handler = +http_event_handler,
        });

        // include the JSON in the body
        esp_http_client_set_header(http_client.get(), "Content-Type", "application/json");
        esp_http_client_set_post_field(http_client.get(), body.data(), body.size());

        // perform HTTP request and return status
        auto err = esp_http_client_perform(http_client.get());
        return err == ESP_OK && !cookie.empty();
    }

    bool log(const std::string& msg) {
        char buffer[128];
        int len = snprintf(buffer, sizeof buffer, R"({
            "message": "%s"
        })", msg.c_str());

        // only use part of the buffer that is written to
        std::string_view body{buffer, static_cast<std::size_t>(len)};

        // without cookie, no access to the logging operation
        if (cookie.empty()) {
            return false;
        }

        static const std::string url = "http://"s.append(web_server_socket).append("api/log");

        static auto http_client = http::client_init({
            .url = url.c_str(),
            .method = HTTP_METHOD_POST,
        });

        // set headers and body
        esp_http_client_set_header(http_client.get(), "Cookie", cookie.c_str());
        esp_http_client_set_header(http_client.get(), "Content-Type", "application/json");
        esp_http_client_set_post_field(http_client.get(), body.data(), body.size());

        // perform HTTP request and return status
        const esp_err_t err = esp_http_client_perform(http_client.get());
        return err == ESP_OK;
    }
}

