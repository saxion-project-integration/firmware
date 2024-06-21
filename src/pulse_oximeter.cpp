/**
 * Implementation of functions based on exmaple by DevXplained (David Weise)
 * https://devxplained.eu/en/blog/max3010x-pulse-oximeter-modules-part-3
*/
#include "../include/pulse_oximeter.h"
#include "pulse_oximeter.h"

Pulse_Oximeter::Pulse_Oximeter() : m_highpass_filter{K_HIGH_PASS_CUTOFF, K_SAMPLING_FREQUENCY},
                                   m_lowpass_filter{K_LOW_PASS_CUTOFF, K_SAMPLING_FREQUENCY},
                                   m_differentiator{K_SAMPLING_FREQUENCY},
                                   m_last_heartbeat{0},
                                   m_finger_timestamp{0},
                                   m_finger_detected{false},
                                   m_last_diff{NAN},
                                   m_crossed{false},
                                   m_crossed_time{0} {
}

bool Pulse_Oximeter::start() {
    return m_sensor.begin() && m_sensor.setSamplingRate(m_sensor.SAMPLING_RATE_400SPS);
}

void Pulse_Oximeter::reset() {
    m_differentiator.reset();
    m_averager.reset();
    m_lowpass_filter.reset();
    m_highpass_filter.reset();

    m_finger_detected = false;
    m_finger_timestamp = millis();
}

float Pulse_Oximeter::filter_sample(uint32_t sample) {
    float current_value = m_lowpass_filter.process(sample);
    current_value = m_highpass_filter.process(current_value);
    return m_differentiator.process(current_value);
}

void Pulse_Oximeter::detect_heartbeat_zero_crossing(float current_diff) {
    if (m_last_diff > 0 && current_diff < 0) {
        m_crossed = true;
        m_crossed_time = millis();
    }
    if (current_diff > 0) m_crossed = false;
}

void Pulse_Oximeter::detect_heartbeat_falling_threshold(float current_diff) {
    if (m_crossed && current_diff < K_EDGE_THRESHOLD) {
        if (m_last_heartbeat != 0 && (m_crossed_time - m_last_heartbeat > 300)) {
            // show results
            int bpm = 60000 / (m_crossed_time - m_last_heartbeat);
            if (bpm > 50 && bpm < 250) {
                if (K_ENABLE_AVERAGING) {
                    m_average_bpm = m_averager.count() > K_SAMPLE_THRESHOLD ? m_averager.process(bpm) : m_average_bpm;
                }
                m_bpm = bpm;
            }
        }
        m_crossed = false;
        m_last_heartbeat = m_crossed_time;
    }
}

void Pulse_Oximeter::detect_finger() {
    if (m_sample.red > K_FINGER_THRESHOLD) {
        if (millis() - m_finger_timestamp > K_FINGER_COOLDOWN_MS) {
            m_finger_detected = true;
        }
    } else {
        reset();
    }
}

bool Pulse_Oximeter::finger_detected() {
    return m_finger_detected;
}

void Pulse_Oximeter::read_sample() {
    m_sample = m_sensor.readSample(1000);
}

uint32_t Pulse_Oximeter::ir_value() {
    return m_sample.ir;
}

uint32_t Pulse_Oximeter::red_value() {
    return m_sample.red;
}

void Pulse_Oximeter::measure_bpm() {
    read_sample();
    detect_finger();
    if (!m_finger_detected) return;
    float current_diff = filter_sample(m_sample.red);
    if (!isnan(current_diff) && !isnan(m_last_diff)) {
        detect_heartbeat_zero_crossing(current_diff);
        detect_heartbeat_falling_threshold(current_diff);
    }
    m_last_diff = current_diff;
}

int Pulse_Oximeter::bpm_value() {
    return m_bpm;
}
