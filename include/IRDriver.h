#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void ir_driver_init(uint8_t pin);
uint32_t ir_driver_read_hex(void);

#ifdef __cplusplus
}
#endif