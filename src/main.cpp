#include <Arduino.h>
#include <fall_detection.h>

void setup() {
    Serial.begin(9600);
    fall_detection::start();
}

void loop() {
    delay(500);
}
