#ifndef ADC_H
#define ADC_H

#include <avr/io.h>

// Initialize the ADC hardware (AVcc reference, 125kHz clock)
void adc_init(void);

// Read any raw 10-bit ADC channel (0 to 7)
uint16_t adc_read(uint8_t channel);

// Read and average multiple samples from any channel (0 to 7)
uint16_t adc_read_averaged(uint8_t channel, uint8_t samples);

#endif // ADC_H