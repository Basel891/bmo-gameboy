#include "IRDriver.h"
#include <Arduino.h>
#include <avr/io.h>

void ir_driver_init(uint8_t pin) {
    // Configure PA7 as input (Clear bit 7 in DDRA)
    DDRA &= ~(1 << DDA7);
    // Optional: Enable internal pull-up by setting PORTA7 high if needed
    // PORTA |= (1 << PORTA7);
}

// Call right after an edge: returns how long the pin stays at `level`
// in microseconds, or 0 on timeout.
static uint32_t ir_measure(uint8_t level, uint32_t timeout_us) {
    uint32_t start = micros();
    // Read PINA directly to check PA7 state
    uint8_t pin_val = (PINA & (1 << PINA7)) ? HIGH : LOW;
    
    while (pin_val == level) {
        if (micros() - start > timeout_us) return 0;
        pin_val = (PINA & (1 << PINA7)) ? HIGH : LOW;
    }
    return micros() - start;
}

uint32_t ir_driver_read_hex(void) {
    // Wait for the falling edge that starts the 9 ms burst on PA7
    uint32_t t0 = micros();
    uint8_t pin_val = (PINA & (1 << PINA7)) ? HIGH : LOW;
    
    while (pin_val == HIGH) {
        if (micros() - t0 > 10000) return 0;
        pin_val = (PINA & (1 << PINA7)) ? HIGH : LOW;
    }

    uint32_t mark = ir_measure(LOW, 10000);
    if (mark < 8000) return 0;

    uint32_t space = ir_measure(HIGH, 6000);
    if (space < 4000) return 0;          // rejects repeat codes (~2.25 ms)

    uint32_t data = 0;
    for (int i = 0; i < 32; i++) {
        if (ir_measure(LOW, 1000) == 0) return 0;      // ~562 us mark

        uint32_t s = ir_measure(HIGH, 2500);            // 562 or 1687 us space
        if (s == 0) return 0;

        data >>= 1;                                     // LSB first
        if (s > 1100) data |= 0x80000000UL;
    }

    // Validate command / ~command
    uint8_t cmd     = (data >> 16) & 0xFF;
    uint8_t cmd_inv = (data >> 24) & 0xFF;
    if ((uint8_t)(cmd ^ cmd_inv) != 0xFF) return 0;

    return data;   // addr | ~addr<<8 | cmd<<16 | ~cmd<<24
}