#ifndef ADC_H
#define ADC_H

#include <avr/io.h>

#define BATTERY_ADC_PIN 6

void adc_init(void);
uint16_t battery_get_mv(void);
uint8_t battery_get_percentage(uint16_t mvolts);

#endif // ADC_H