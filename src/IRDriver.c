#include "IRDriver.h"
#include "timer.h"
#include "global.h"
#include <avr/io.h>

// ---- IR receiver pin configuration (defaults defined in IRDriver.h) ----
#ifndef IR_DDR
#define IR_DDR    DDRD
#endif

#ifndef IR_PORT
#define IR_PORT   PORTD
#endif

#ifndef IR_PIN
#define IR_PIN    PIND
#endif

#ifndef IR_PINR
#define IR_PINR   IR_PIN
#endif

#ifndef IR_BIT
#define IR_BIT    PD7
#endif
// -----------------------------------------------------------

static inline uint8_t ir_read(void) {
    return (IR_PIN & (1 << IR_BIT)) ? HIGH : LOW;
}

void ir_driver_init(uint8_t pin) {
    (void)pin;                      // pin is fixed at compile time via IR_* macros (default: PD7)
    IR_DDR &= ~(1 << IR_BIT);       // input
    IR_PORT &= ~(1 << IR_BIT);      // pull-up off (VS1838/TSOP drive the line themselves)
}

// Call right after an edge: returns how long the pin stays at `level`
// in microseconds, or 0 on timeout.
static uint32_t ir_measure(uint8_t level, uint32_t timeout_us) {
    uint32_t start = micros();
    while (ir_read() == level) {
        if (micros() - start > timeout_us) return 0;
    }
    return micros() - start;
}

uint32_t ir_driver_read_hex(void) {
    // Wait for the falling edge that starts the 9 ms burst
    uint32_t t0 = micros();
    while (ir_read() == HIGH) {
        if (micros() - t0 > 10000) return 0;
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