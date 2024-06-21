/**
 * Created by Joran Hagen - May 2024
*/

#ifndef PULSE_OXIMETER_H
#define PULSE_OXIMETER_H

#include "filters.h"
#include <MAX3010x.h>

typedef struct MAX30102Sample t_sample;

// constants
const float K_SAMPLING_FREQUENCY = 400.0;
const unsigned long K_FINGER_THRESHOLD = 10000;
const unsigned int K_FINGER_COOLDOWN_MS = 500;
const float K_EDGE_THRESHOLD = -2000.0;
const float K_LOW_PASS_CUTOFF = 5.0;
const float K_HIGH_PASS_CUTOFF = 0.5;
const bool K_ENABLE_AVERAGING = true;
const int K_AVERAGING_SAMPLES = 50;
const int K_SAMPLE_THRESHOLD = 5;

class Pulse_Oximeter {
private:
    MAX30102 m_sensor;
    // filters
    HighPassFilter m_highpass_filter;
    LowPassFilter m_lowpass_filter;
    // differentiator
    Differentiator m_differentiator;
    // moving average filter
    MovingAverageFilter<K_AVERAGING_SAMPLES> m_averager;
    // sample values
    t_sample m_sample;
    // timestamp of last heartbeat
    long m_last_heartbeat;
    // timestamp for finger detection
    long m_finger_timestamp;
    bool m_finger_detected;
    // last diff to detect zero crossing
    float m_last_diff;
    bool m_crossed;
    long m_crossed_time;
    // measure bpms
    int m_average_bpm;
    int m_bpm;
    // functions
    void reset();
    void detect_finger();
    float filter_sample(uint32_t);

    void detect_heartbeat_zero_crossing(float current_diff);
    void detect_heartbeat_falling_threshold(float current_diff);

public:
    Pulse_Oximeter(/* args */);

    bool start();
    bool finger_detected();

    void read_sample();
    void measure_bpm();

    uint32_t ir_value();
    uint32_t red_value();
    int bpm_value();
    
};

#endif /* PULSE_OXIMETER_H */
