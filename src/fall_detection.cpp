/**
 * @file fall_detection.cpp
 * @brief Contains function(s) that deal with fall detection.
 */

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <span>
#include <type_traits>

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include <fall_detection.h>
#include <log.h>

namespace fall_detection {
    namespace {
        TaskHandle_t monitor_handle{};
        Adafruit_MPU6050 mpu{};

        constexpr std::uint8_t sda_pin = 15;
        constexpr std::uint8_t scl_pin = 16;
        constexpr std::size_t interval_in_ms = 500;
        constexpr std::size_t fall_timeout_in_ms = 5000;
        constexpr float acceleration_treshold = 14.0f;

        /**
         * Initializes the MPU-6050.
         */
        void init_mpu() {
            Wire.setPins(sda_pin, scl_pin);

            while (!mpu.begin()) {
                delay(10);
            }

            mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
            mpu.setGyroRange(MPU6050_RANGE_500_DEG);
            mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
            delay(100);
        }

        /**
         * Magnitude of vector.
         *
         * @param vec Vector to calculate the magnitude of.
         * @return Magnitude of vector.
         */
        template<typename T, std::size_t N>
        requires std::is_arithmetic_v<T>
        T magnitude(const T (&vec)[N]) {
            return std::sqrt(std::accumulate(std::begin(vec), std::end(vec), T{}, [](auto sum, auto num) {
                return sum + num * num;
            }));
        }

        /**
         * Monitors readings from the MPU-6050 in order to detect falls.
         */
        void monitor(void*) {
            while (true) {
                static sensors_event_t a, g, temp;
                mpu.getEvent(&a, &g, &temp);
                auto vec = a.acceleration.v;

                if (const auto& vec = a.acceleration.v; magnitude(vec) > acceleration_treshold) {
                    Serial.println("Fall detected.");
                    pi::log("fall detected");
                    delay(fall_timeout_in_ms);
                }

                delay(interval_in_ms);
            }
        }
    }

    void start() {
        init_mpu();
        xTaskCreate(monitor, "monitor", 4000, nullptr, 1, &monitor_handle);
    }
}

