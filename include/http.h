/**
 * @file http.h
 * @brief Contains a RAII wrapper for a HTTP client handle.
 */

#ifndef HTTP_H
#define HTTP_H

#include <memory>

#include "esp_http_client.h"

namespace http {
    /**
     * Function object for deleting HTTP client handles.
     */
    struct client_deleter {
        inline void operator()(esp_http_client_handle_t client) {
            esp_http_client_cleanup(client);
        }
    };

    /**
     * A RAII wrapper for a HTTP client handle.
     */
    using client_handle = std::unique_ptr<esp_http_client, client_deleter>;

    /**
     * Constructs an `esp_http_client_handle_t` and wraps it in a `client_handle`.
     *
     * @param config Configuration for the to be constructed HTTP client.
     * @return The newly constructed `client_handle`.
     */
    inline client_handle client_init(const esp_http_client_config_t& config = {}) {
        return client_handle{esp_http_client_init(&config)};
    }
}

#endif

