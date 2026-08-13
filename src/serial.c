#include "serial.h"

void serial_init(uint32_t baud_rate)
{
    // Calculate the Baud Rate Register (UBRR) value for Normal Speed Mode
    uint16_t ubrr_val = (F_CPU / (16UL * baud_rate)) - 1;

    // Set baud rate registers
    UBRR0H = (uint8_t)(ubrr_val >> 8);
    UBRR0L = (uint8_t)(ubrr_val);

    // Enable both Transmitter (TX) and Receiver (RX)
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    // Set frame format: 8 data bits, 1 stop bit, no parity (8N1)
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    serial_print("Serial Monitor Initialized!");
}

void uart_transmit(uint8_t data)
{
    // Wait until the transmit buffer is empty (UDRE0 bit becomes 1)
    while (!(UCSR0A & (1 << UDRE0)))
        ;

    // Put data into the buffer, which sends the data
    UDR0 = data;
}

uint8_t uart_receive(void)
{
    // Wait until data is completely received (RXC0 bit becomes 1)
    while (!(UCSR0A & (1 << RXC0)))
        ;

    // Get and return received data from buffer
    return UDR0;
}

void serial_print(const char *str)
{
    // Loop through the string until the null-terminator is reached
    while (*str)
    {
        // If it's a newline, add a carriage return first for the Serial Monitor to reset cursor position to the beginning of the line
        if (*str == '\n')
        {
            uart_transmit('\r');
        }
        uart_transmit(*str++);
    }

    // Reset cursor position and start a new line at the end of the print
    uart_transmit('\r');
    uart_transmit('\n');
}