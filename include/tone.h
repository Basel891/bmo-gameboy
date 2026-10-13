#ifndef TONE_H
#define TONE_H

#include <stdint.h>

/**
 * @brief Initialize Timer1 peripheral and pin direction for hardware tone generation.
 */
void tone_init(void);

/**
 * @brief Drop-in replacement for Arduino tone().
 * @param pin output buzzer pin
 * @param frequency Target tone frequency in Hz (Pass 0 to stop output).
 * @param duration Unused (kept for compatibility). Duration is managed by your RTTTL loop.
 */
void tone(uint8_t pin, uint16_t frequency, unsigned long duration);

/**
 * @brief Drop-in replacement for Arduino noTone(). Stops active output on Timer1.
 * @param pin output buzzer pin
 */
void noTone(uint8_t pin);

#endif // TONE_H