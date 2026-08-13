#pragma once

#include <avr/io.h>

/**
 * @brief Initializes the Serial monitor and the UART Communication protocol over Tx and Rx
 *
 * @param baud_rate number of signal state changes per second (symbols/sec) 9600, 19200, 38400, 57600, 115200 Bd are all common values
 *
 * @note Call it in int main() before the main loop
 * @note If you don't know what value to put for the baud_rate stick to 9600 as the default
 */
void serial_init(uint32_t baud_rate);

void uart_transmit(uint8_t data);

uint8_t uart_receive(void);

/**
 * @brief Prints The messages from the "str" param to the Serial Monitor terminal
 *
 * @param str data string
 */
void serial_print(const char *str);