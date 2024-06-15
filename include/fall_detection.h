/**
 * @file fall_detection.h
 * @brief Contains function(s) that deal with fall detection.
 */

#ifndef FALL_DETECTION_H
#define FALL_DETECTION_H

namespace fall_detection {
    /**
     * Start detecting falls.
     *
     * This function will start repeatedly monitor the accelerometer for and try to detect falls.
     */
    void start();
}

#endif
