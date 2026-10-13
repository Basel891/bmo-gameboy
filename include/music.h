#ifndef MUSIC_H
#define MUSIC_H

#include <avr/io.h>

#define tonePin_Arduino PD3 // D3 on arduino nano
// New non-blocking functions
void start_song(const char *p);
void update_music(void);

#endif