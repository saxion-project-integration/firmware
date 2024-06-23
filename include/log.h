/**
 * @file log.h
 * @brief Contains function(s) for logging messages to the database.
 */

#ifndef LOG_H
#define LOG_H

#include <string>
#include <string_view>

namespace pi {
    /**
     * Log in to the web server.
     *
     * This function attempts to log in to the web server sets `cookie` to the returned session cookie.
     *
     * @param username The username to log in with.
     * @param password The password to log in with.
     * @return `true` if logged in successfully, `false` otherwise.
     */
    bool login(const std::string& username, const std::string& password);

    /**
     * Log message to the database.
     *
     * This function logs a message to the database by making a HTTP request to the web server.
     *
     * @param msg The message to log.
     * @return `true` if the operation was performed successfully, `false` otherwise.
     */
    bool log(const std::string& msg);

    /*
     * Log heart rate to the database.
     *
     * This function logs a heart rate to the database by making a HTTP request to the web server.
     *
     * @param bpm The heart rate in beats per minute.
     * @return `true` if the operation was performed successfully, `false` otherwise.
     */
    bool log_heart_rate(int bpm);
}

#endif

