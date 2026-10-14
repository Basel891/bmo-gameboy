#include "IRDriver.h"
#include <Arduino.h>

static uint8_t ir_pin;

void ir_driver_init(uint8_t pin) {
    ir_pin = pin;
    pinMode(ir_pin, INPUT);
}

uint32_t ir_driver_read_hex(void) {
    // Wait for the 9ms leading LOW pulse (NEC protocol)
    if (pulseIn(ir_pin, LOW, 10000) < 8000) {
        return 0; 
    }
    
    // Wait for the 4.5ms HIGH space
    if (pulseIn(ir_pin, HIGH, 6000) < 3000) {
        return 0;
    }

    uint32_t data = 0;
    // Read 32 bits of NEC protocol data
    for (int i = 0; i < 32; i++) {
        if (pulseIn(ir_pin, LOW, 2000) == 0) {
            return 0;
        }

        unsigned long high_pulse = pulseIn(ir_pin, HIGH, 3000);
        data = data << 1;
        if (high_pulse > 1000) {
            data |= 1;
        }
    }
    
    return data;
}