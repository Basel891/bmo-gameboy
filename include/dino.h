#pragma once


#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "controller.h"

#define DINO_SCREEN_WIDTH   160
#define DINO_SCREEN_HEIGHT  128

#define GROUND_Y 105
#define DINO_X_POS 20
#define GRAVITY 1
#define JUMP_IMPULSE -8

#define DINO_W 16     // Width of Dino sprite
#define DINO_H 16     // Height of Dino sprite
#define OBSTACLE_W 8  // Width of Cactus sprite
#define OBSTACLE_H 16 // Height of Cactus sprite

typedef enum
{
    GAME_STATE_START,     // Waiting at start screen for player to press PC4
    GAME_STATE_RUNNING,   // Active gameplay in progress
    GAME_STATE_GAME_OVER, // Player collided with obstacle, displaying loss panel
    GAME_STATE_EXIT       // Player pressed PC5 to quit back to main menu
} GameState;

typedef struct
{
    int16_t x, y;         // Current screen coordinates
    int16_t old_x, old_y; // Previous frame coordinates (used for dirty rect clearing)
    int8_t vy;            // Current vertical velocity
    bool is_jumping;      // Flag preventing double-jumping while in mid-air
} PlayerDino;

typedef struct
{
    int16_t x, y;         // Current screen coordinates
    int16_t old_x, old_y; // Previous frame coordinates (used for dirty rect clearing)
    bool active;          // Obstacle active flag
} Obstacle;

void play_dino(void);