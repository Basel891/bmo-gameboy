#include "tone.h"
#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// tone.c library based on the built in tone methods normally imported from the arduino standard library

// Hardware fixed output pin: PB1 (ATmega328P Pin 15 / Arduino Pin 9 - OC1A)
#define TONE_DDR DDRB
#define TONE_PORT PORTB
#define TONE_PIN PB1

void tone_init(void)
{
    // Set PB1 (OC1A) as an output pin
    TONE_DDR |= (1 << TONE_PIN);

    // Reset Timer1 registers to a known stopped state
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
}

void tone(uint8_t pin, uint16_t frequency, unsigned long duration)
{
    (void)pin;      // Silence unused variable warning
    (void)duration; // Duration managed by RTTTL state machine

    if (frequency == 0)
    {
        noTone(0);
        return;
    }

    // COM1A1:0 = 01 -> Toggle OC1A (PB1) on Compare Match
    TCCR1A = (1 << COM1A0);

    /*
     * CTC Formula:
     * Toggle Freq = F_CPU / (2 * Prescaler * (1 + OCR1A))
     * ==> OCR1A   = (F_CPU / (2 * Prescaler * Target_Freq)) - 1
     *
     * Using Prescaler = 64 (Supports frequencies down to ~15 Hz @ 16 MHz F_CPU)
     */
    uint32_t ocr_value = (F_CPU / (2UL * 64UL * (uint32_t)frequency)) - 1UL;

    // Safety clamp for 16-bit Timer1 max limit
    if (ocr_value > 65535)
    {
        ocr_value = 65535;
    }

    OCR1A = (uint16_t)ocr_value;

    // Clear counter register to prevent skipped match glitches
    TCNT1 = 0;

    // WGM13:0 = 0100 (CTC Mode with OCR1A top)
    // CS12:0  = 011  (Prescaler 64) -> Starts the clock generator
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
}

void noTone(uint8_t pin)
{
    (void)pin;

    // Stop Timer1 clock source
    TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));

    // Disconnect OC1A hardware toggling from PB1
    TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));

    // Force pin LOW to avoid floating / DC offset on piezo
    TONE_PORT &= ~(1 << TONE_PIN);
}