#pragma once

#include <avr/io.h>
#include <BIT_MATH.h>

#define CONTROLLER_PORT PORTC
#define CONTROLLER_PIN PINC
#define CONTROLLER_DDR DDRC

#define RIGHT PC0
#define LEFT PC1
#define UP PC2
#define DOWN PC3
#define START PC4
#define BACK PC5

/* Button pins are active low because we will use the internal pullups of the pins by setting the DDR as INPUT and making the PORT Pins HIGH */

/**
 * @brief Initializes the DDR direction for the controller keys and sets up the internal pullups for each key
 *
 * @note Put it before the main loop
 */
void controller_init();

/**
 * @brief Checks if the key passed was pressed or not
 *
 * @param key takes key pin number
 * @return uint8_t Returns 0 (False) or 1 (True) based if the "key" passed was pressed or not
 *
 * @note To be used inside an if condition in the main loop to check if that key was pressed or not and then perform actions based on the press
 */
uint8_t get_key(uint8_t key);