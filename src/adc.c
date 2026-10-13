#include <avr/io.h>
#include "adc.h"

void adc_init(void)
{
    // Select AVcc (+5V) as reference
    ADMUX = (1 << REFS0);

    // Enable ADC and set prescaler to 128 (125 kHz @ 16 MHz)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adc_read(uint8_t channel)
{
    // Clear lower 4 bits (MUX3:0) and set channel (0 to 7)
    ADMUX = (ADMUX & 0xF0) | (channel & 0x07);

    // Start single conversion
    ADCSRA |= (1 << ADSC);

    // Wait until conversion completes
    while (ADCSRA & (1 << ADSC))
        ;

    // Return 10-bit result
    return ADC;
}

uint16_t adc_read_averaged(uint8_t channel, uint8_t samples)
{
    if (samples == 0)
        samples = 1;

    uint32_t sum = 0;
    for (uint8_t i = 0; i < samples; i++)
    {
        sum += adc_read(channel);
    }
    return (uint16_t)(sum / samples);
}