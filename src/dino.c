#include "dino.h"
#include <stdlib.h>
#include <stdio.h>
#include "TFT.h"

const uint8_t PROGMEM dino_frame0[] = {
    0x07, 0xC0, 0x0F, 0xE0, 0x0F, 0xE0, 0x0E, 0x00, 0x1F, 0xE0, 0x1F, 0xF0,
    0x1F, 0xF8, 0x0F, 0xFC, 0x07, 0xFE, 0x03, 0xFC, 0x03, 0xF8, 0x01, 0xF0,
    0x00, 0xE0, 0x00, 0xC0, 0x00, 0x80, 0x00, 0x80
};

const uint8_t PROGMEM cactus_sprite[] = {
    0x18, 0x18, 0x18, 0x5A, 0x5A, 0x5A, 0x5A, 0x3C, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18
};                                              

static PlayerDino dino;
static Obstacle cactus;
static uint16_t score = 0;
static uint16_t old_score = 0xFFFF; // Forces HUD redraw on start
static uint8_t scroll_speed = 3;
static GameState current_state = GAME_STATE_START;
static GameState rendered_state = (GameState)0xFF; // Prevents continuous redrawing of UI screens

static void DrawHUDScore(void) {
    if (score != old_score) {
        char score_str[7];
        utoa(score, score_str, 10);

        // Clear score numerical area first to prevent trailing digits from old scores
        TFT_FillRect(135, 5, 25, 8, 0x0000);
        TFT_DrawString(95, 5, "SCORE:", 0x07E0, 0x0000,1); 
        TFT_DrawString(135, 5, score_str, 0xFFFF, 0x0000,1);

        old_score = score;
    }
}

void DinoGame_Init(void) {
    current_state = GAME_STATE_START;
    rendered_state = (GameState)0xFF;
    TFT_FillRect(0, 0, DINO_SCREEN_WIDTH, DINO_SCREEN_HEIGHT, 0x0000);
}

void DinoGame_Reset(void) {
    // Reset Dino
    dino.x = DINO_X_POS;
    dino.y = GROUND_Y - DINO_H;
    dino.old_x = dino.x;
    dino.old_y = dino.y;
    dino.vy = 0;
    dino.is_jumping = false;

    // Reset Cactus
    cactus.x = DINO_SCREEN_WIDTH + 20;
    cactus.y = GROUND_Y - OBSTACLE_H;
    cactus.old_x = cactus.x;
    cactus.old_y = cactus.y;
    cactus.active = true;

    // Reset Score & Physics Speed
    score = 0;
    old_score = 0xFFFF;
    scroll_speed = 3;
    current_state = GAME_STATE_RUNNING;
    rendered_state = GAME_STATE_RUNNING;

    // Full Screen Clear
    TFT_FillRect(0, 0, DINO_SCREEN_WIDTH, DINO_SCREEN_HEIGHT, 0x0000);
}

void DinoGame_ProcessInput(void) {
    // 1. Checked constantly every frame in every state
    if (get_key(BACK)) {
        current_state = GAME_STATE_EXIT;
        return;
    }

    // 2. Filter inputs based on active game state
    switch (current_state) {
        case GAME_STATE_START:
        case GAME_STATE_GAME_OVER:
            // Pressing START on either screen starts/restarts the game
            if (get_key(START)) {
                DinoGame_Reset();
            }
            break;

        case GAME_STATE_RUNNING:
            // ONLY the UP button triggers a jump
            if (get_key(UP) && !dino.is_jumping) {
                dino.vy = JUMP_IMPULSE;
                dino.is_jumping = true;
            }
            break;

        default:
            break;
    }
}

void DinoGame_UpdatePhysics(void) {
    if (current_state != GAME_STATE_RUNNING) return;

    // Preserve previous frame positions for dirty rendering
    dino.old_y = dino.y;
    cactus.old_x = cactus.x;

    // 1. Dino Jump & Gravity Acceleration
    dino.y += dino.vy;
    dino.vy += GRAVITY;

    if (dino.y >= GROUND_Y - DINO_H) {
        dino.y = GROUND_Y - DINO_H;
        dino.vy = 0;
        dino.is_jumping = false;
    }

    // 2. Obstacle Movement
    cactus.x -= scroll_speed;

    // 3. Obstacle Respawn & Score Increase
    if (cactus.x + OBSTACLE_W < 0) {
        cactus.x = DINO_SCREEN_WIDTH + (rand() % 40);
        score += 10;

        // Increase scroll speed as score rises
        if (score % 50 == 0 && scroll_speed < 8) {
            scroll_speed++;
        }
    }
}

bool DinoGame_CheckCollision(void) {
    if (current_state != GAME_STATE_RUNNING) return false;

    if (dino.x < cactus.x + OBSTACLE_W &&
        dino.x + DINO_W > cactus.x &&
        dino.y < cactus.y + OBSTACLE_H &&
        dino.y + DINO_H > cactus.y) {
        current_state = GAME_STATE_GAME_OVER;
        return true;
    }
    return false;
}

void DinoGame_Render(void) {
    if (current_state == GAME_STATE_START) {
        if (rendered_state != GAME_STATE_START) {
            TFT_FillRect(0, 0, DINO_SCREEN_WIDTH, DINO_SCREEN_HEIGHT, 0x0000);
            TFT_DrawString(35, 40, "DINO RUNNER", 0xFFE0, 0x0000,1);
            TFT_DrawString(30, 70, "START: Play", 0x07E0, 0x0000,1);
            TFT_DrawString(30, 85, "BACK: Exit", 0xF800, 0x0000,1);
            rendered_state = GAME_STATE_START;
        }
        return;
    }

    if (current_state == GAME_STATE_GAME_OVER) {
        if (rendered_state != GAME_STATE_GAME_OVER) {
            // Draw Game Over Panel Overlay ONCE
            TFT_FillRect(20, 30, 120, 70, 0x0000);
            TFT_DrawString(40, 40, "GAME OVER", 0xF800, 0x0000,1);
            TFT_DrawString(25, 65, "START: Restart", 0x07E0, 0x0000,1);
            TFT_DrawString(25, 80, "BACK: Exit", 0xFFFF, 0x0000,1);
            rendered_state = GAME_STATE_GAME_OVER;
        }
        return;
    }

    if (current_state == GAME_STATE_RUNNING) {
        // 1. Clear old dino bounds
        if (dino.old_y != dino.y) {
            TFT_FillRect(dino.old_x, dino.old_y, DINO_W, DINO_H, 0x0000);
        }

        // 2. Clear old cactus bounds (handles normal movement AND teleport respawn)
        if (cactus.old_x != cactus.x) {
            if (cactus.old_x < cactus.x) {
                // Respawned to the right: erase entire old bounding box on left
                if (cactus.old_x >= -OBSTACLE_W && cactus.old_x < DINO_SCREEN_WIDTH) {
                    TFT_FillRect(cactus.old_x, cactus.y, OBSTACLE_W, OBSTACLE_H, 0x0000);
                }
            } else {
                // Normal leftward scroll clear
                int16_t clear_w = cactus.old_x - cactus.x;
                if (clear_w > 0 && cactus.old_x >= 0) {
                    TFT_FillRect(cactus.old_x + OBSTACLE_W - clear_w, cactus.y, clear_w, OBSTACLE_H, 0x0000);
                }
            }
        }

        // 3. Draw Ground Line
        TFT_FillRect(0, GROUND_Y, DINO_SCREEN_WIDTH, 1, 0xFFFF);

        // 4. Render Dino Sprite
        TFT_DrawBitmap1Bit_Scaled_Clipped(
            dino.x, dino.y, dino_frame0, 
            DINO_W, DINO_H, 0x07E0, 0x0000, 1
        );

        // 5. Render Cactus Sprite
        TFT_DrawBitmap1Bit_Scaled_Clipped(
            cactus.x, cactus.y, cactus_sprite, 
            OBSTACLE_W, OBSTACLE_H, 0xF800, 0x0000, 1
        );

        // 6. Draw HUD Score
        DrawHUDScore();
    }
}

GameState DinoGame_GetState(void) {
    return current_state;
}