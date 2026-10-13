#ifndef MENU_H
#define MENU_H

#include <stdint.h>

#define MENU_TEXT 0x0280
#define MENU_BACKGROUND 0x76EF

void menu(void);
void menu_down(void);
void menu_up(void);
uint8_t menu_get_selected(void);

#endif