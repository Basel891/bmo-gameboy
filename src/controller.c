#include "controller.h"
#include <util/delay.h>
#include <IRDriver.h>

static uint8_t previous_states = 0b00000000;

void controller_init()
{
    // Sets pins DDR direction (0 as input)
    CLR_BIT(CONTROLLER_DDR, RIGHT);
    CLR_BIT(CONTROLLER_DDR, LEFT);
    CLR_BIT(CONTROLLER_DDR, UP);
    CLR_BIT(CONTROLLER_DDR, DOWN);
    CLR_BIT(CONTROLLER_DDR, START);
    CLR_BIT(CONTROLLER_DDR, BACK);

    // Set the Pins as HIGH to enable the internal Pull-ups
    SET_BIT(CONTROLLER_PORT, RIGHT);
    SET_BIT(CONTROLLER_PORT, LEFT);
    SET_BIT(CONTROLLER_PORT, UP);
    SET_BIT(CONTROLLER_PORT, DOWN);
    SET_BIT(CONTROLLER_PORT, START);
    SET_BIT(CONTROLLER_PORT, BACK);
}

uint8_t get_key(uint8_t key)
{
    // Active-LOW  pin reads 0 when pressed, invert so 1 = pressed
    uint8_t is_pressed = !GET_BIT(CONTROLLER_PIN, key);
    uint8_t was_pressed = GET_BIT(previous_states, key);

    // Initial press detected (1 now, 0 before)
    if (is_pressed && !was_pressed)
    {
        SET_BIT(previous_states, key); // Mark as pressed
        return 1;
    }

    // Button released (0 now)
    if (!is_pressed)
    {
        CLR_BIT(previous_states, key); // Reset state for next press
    }

    return 0; // Returns 0 if holding continuously or if not pressed
}
uint32_t get_key_signal(void)
{
    static uint32_t last_received_hex = 0;
    static uint8_t button_locked = 0;

    // Define the hex codes for the buttons
    const uint32_t UP_BUTTON     = 0xE0E006F9;
    const uint32_t DOWN_BUTTON   = 0xE0E08679;
    const uint32_t LEFT_BUTTON   = 0xE0E0A659;
    const uint32_t RIGHT_BUTTON  = 0xE0E046B9;
    const uint32_t RETURN_BUTTON = 0xE0E01AE5;

    uint32_t current_hex = ir_driver_read_hex();

    // If no signal is received (button released/idle), reset the lock
    if (current_hex == 0)
    {
        button_locked = 0;
        last_received_hex = 0;
        return 0;
    }

    // If the same button is held down, ignore subsequent triggers until released
    if (current_hex == last_received_hex && button_locked)
    {
        return 0;
    }

    // New button press detected
    last_received_hex = current_hex;
    button_locked = 1; // Lock until the driver returns 0 (released)

    // Check which button code was received
    switch (current_hex)
    {
        case UP_BUTTON:
            return 1; // Up pressed
        case DOWN_BUTTON:
            return 2; // Down pressed
        case LEFT_BUTTON:
            return 3; // Left pressed
        case RIGHT_BUTTON:
            return 4; // Right pressed
        case RETURN_BUTTON:
            return 5; // Return pressed
        default:
            button_locked = 0; // Unknown code, unlock
            return 0;          // No matching button
    }
}