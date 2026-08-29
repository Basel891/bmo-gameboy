#include <avr/io.h>

#include "menu.h"
#include "TFT.h"

static uint8_t selected_game = 0;

void menu(void)
{
    TFT_DrawString(20, 20, "BMO game", TFT_RED, TFT_BLACK, 1);

    if (selected_game == 0)
        TFT_DrawString(5, 50, ">>", TFT_RED, TFT_BLACK, 1);
    else
        TFT_DrawString(5, 50, "  ", TFT_RED, TFT_BLACK, 1);

    TFT_DrawString(20, 50, "TETRIS", TFT_RED, TFT_BLACK, 1);

    if (selected_game == 1)
        TFT_DrawString(5, 80, ">>", TFT_RED, TFT_BLACK, 1);
    else
        TFT_DrawString(5, 80, "  ", TFT_RED, TFT_BLACK, 1);

    TFT_DrawString(20, 80, "snake", TFT_RED, TFT_BLACK, 1);

    if (selected_game == 2)
        TFT_DrawString(5, 110, ">>", TFT_RED, TFT_BLACK, 1);
    else
        TFT_DrawString(5, 110, "  ", TFT_RED, TFT_BLACK, 1);

    TFT_DrawString(20, 110, "pong", TFT_RED, TFT_BLACK, 1);
}

void menu_down(void)
{
    if (selected_game < 2)
    {
        selected_game++;
    }
}

void menu_up(void)
{
    if (selected_game > 0)
    {
        selected_game--;
    }
}

uint8_t menu_get_selected(void)
{
    return selected_game;
}