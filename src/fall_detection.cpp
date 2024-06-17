/**
 * @file fall_detection.cpp
 * @brief Contains function(s) that deal with fall detection.
 */

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include <cstdint>
#include <cmath>
#include <fall_detection.h>
#include <log.h>

namespace fall_detection {
    namespace {
        TaskHandle_t monitor_handle{};
        Adafruit_MPU6050 mpu{};

        constexpr std::uint8_t sda_pin = 21;
        constexpr std::uint8_t scl_pin = 22;
        constexpr std::size_t interval_in_ms = 500;
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
         * Monitors readings from the MPU-6050 in order to detect falls.
         */
        void monitor(void*) {
            while (true) {
                static sensors_event_t a, g, temp;
                mpu.getEvent(&a, &g, &temp);

                auto v = a.acceleration.v;
                auto magnitude = std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);

                if (magnitude > acceleration_treshold) {
                    Serial.println("Fall detected.");
                    pi::log("fall detected");
                }

                delay(interval_in_ms);
            }
        }
    }

    void start() {
        init_mpu();
        xTaskCreate(monitor, "monitor", 2056, nullptr, 1, &monitor_handle);
    }
}

