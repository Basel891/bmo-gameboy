#pragma once
#include <stdint.h>
#include <avr/io.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---- IR receiver pin configuration ----
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
// ---------------------------------------

void ir_driver_init(uint8_t pin);
uint32_t ir_driver_read_hex(void);

#ifdef __cplusplus
}
#endif