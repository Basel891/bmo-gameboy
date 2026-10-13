#include "adc.h"
#include <avr/io.h>
#include <stdio.h>

void adc_init(void)
{
    // 1. Select AVcc (+5V) as reference voltage (REFS1:0 = 01)
    ADMUX = (1 << REFS0);

    // 2. Enable ADC (ADEN) and set Prescaler to 128 (16 MHz / 128 = 125 kHz)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t battery_get_mv(void)
{
    // Select Channel 6 (ADC6 / A6) by clearing MUX3:0 and setting MUX2 | MUX1 (0110 = Channel 6)
    ADMUX = (ADMUX & 0xF0) | (BATTERY_ADC_PIN & 0x0F);

    // Start Conversion
    ADCSRA |= (1 << ADSC);

    // Wait for conversion to complete
    while (ADCSRA & (1 << ADSC))
        ;

    // Read 10-bit raw result from ADC data register
    uint32_t raw_adc = ADC;

    // Calculate voltage in mV:
    // V_adc = (raw_adc * 5000) / 1024
    // V_bat = V_adc * 2 (100k/100k divider cuts voltage in half)
    uint32_t battery_mv = ((raw_adc * 10000UL) + 512UL) / 1024UL;

    return (uint16_t)battery_mv;
}

uint8_t battery_get_percentage(uint16_t mvolts)
{
    if (mvolts <= 6000)
        return 0;
    if (mvolts >= 8350)
        return 100;

    // Map 6.0V (0%) to 8.4V (100%)
    return (uint8_t)(((uint32_t)(mvolts - 6000) * 100) / (8350 - 6000));
}