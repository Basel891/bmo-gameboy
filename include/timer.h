#pragma once

#include <avr/io.h>

void timer_init();
uint32_t millis();
int8_t timer_set_timeout(uint32_t delay_ms, void (*cb)(void));
int8_t timer_set_interval(uint32_t delay_ms, void (*cb)(void));
void timer_cancel(uint8_t timer_id);
void timer_update();
