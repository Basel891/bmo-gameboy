#ifndef MUSIC_H
#define MUSIC_H

#include <Arduino.h>

#define tonePin_Arduino 10

// New non-blocking functions
void start_song(const char *p);
void update_music(void);

#endif