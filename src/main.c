#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#include "controller.h"
#include "TFT.h"
#include "SPI.h"
#include "menu.h"
#include "dino.h"

typedef enum
{
    MENU,
    TETRIS,
    SNAKE,
    PONG,
    DINO
} GAMESTATE;

int main(void)
{
    SPI_Init();
    TFT_INIT();
    controller_init();

    TFT_FillScreen(TFT_BLACK);

    TFT_DrawString(20, 20, "BMO game", TFT_RED, TFT_BLACK, 1);

    while (!get_key(START))
    {
    
    }

    TFT_FillScreen(TFT_BLACK);

    menu();

   GAMESTATE state = MENU;

    while (true)
    {
        if (state == MENU)
        {
            if (get_key(DOWN))
            {
                menu_down();
                menu();
            }

            if (get_key(UP))
            {
                menu_up();
                menu();
            }

            if (get_key(START))
            {
                TFT_FillScreen(TFT_BLACK);

                if (menu_get_selected() == 0)
                {
                    state = TETRIS;
                    TFT_DrawString(20, 60, "TETRIS", TFT_RED, TFT_BLACK, 1);
                }
                else if (menu_get_selected() == 1)
                {
                    state = SNAKE;
                    TFT_DrawString(20, 60, "snake", TFT_RED, TFT_BLACK, 1);
                }
                else if (menu_get_selected() == 2)
                {
                    state = PONG;
                    TFT_DrawString(20, 60, "pong", TFT_RED, TFT_BLACK, 1);
                }
                else if (menu_get_selected() == 3)
                {
                    play_dino();
                    TFT_FillScreen(TFT_BLACK);
                    menu();
                }
            }
        }
        else
        {
            if (get_key(BACK))
            {
                state = MENU;
                TFT_FillScreen(TFT_BLACK);
                menu();
            }
        }
    }
}