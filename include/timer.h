#pragma once

#include <avr/io.h>
#include <stddef.h>

void timer_init();
uint32_t millis();
int8_t timer_set_timeout(uint32_t delay_ms, void (*cb)(void *arg), void *arg);
int8_t timer_set_interval(uint32_t delay_ms, void (*cb)(void *arg), void *arg);
void timer_update();
