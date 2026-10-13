#pragma once

#include <avr/io.h>

/**
 * @brief Initializes the ADC hardware and resets internal battery tracking state.
 */
void battery_init(void);

/**
 * @brief Forces the battery status header to redraw on the next update call
 *        (e.g., when exiting a game or returning to the MENU screen).
 */
void battery_force_redraw(void);

/**
 * @brief Updates and renders the battery header if enough time has passed
 *        and the voltage/percentage has changed (non-blocking).
 * @param current_time Current millis() value.
 */
void battery_update(uint32_t current_time);