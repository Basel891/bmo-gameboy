/*
This code is based on bluejay's RTTL parser rewritten in c instead of javascript.

This version is blocking and uses delays opposite to the other music.c which works as a state machine and uses
hardware timers.
*/

#include <math.h>
#include "tone.h"
#include "util/delay.h"
#include "music.h"
#include <stdbool.h>
#include "global.h"
#include <string.h>
#define isdigit(n) (n >= '0' && n <= '9')

void play_rtttl(const char *p)
{
    // Crucial Guard: Ensure the colon exists before jumping blindly
    if (!strchr(p, ':'))
        return;

    uint16_t bpm = 63;
    uint8_t default_oct = 6;
    uint8_t default_dur = 4;

    // 1. Skip Name Section
    while (*p != ':' && *p != '\0')
        p++;
    if (*p == ':')
        p++;

    // 2. Parse Defaults Section (Bluejay style)
    while (*p != ':' && *p != '\0')
    {
        if (*p == 'd')
        {
            p += 2;
            int num = 0;
            while (isdigit(*p))
            {
                num = (num * 10) + (*p++ - '0');
            }
            if (num > 0)
                default_dur = num;
        }
        else if (*p == 'o')
        {
            p += 2;
            int num = 0;
            while (isdigit(*p))
            {
                num = (num * 10) + (*p++ - '0');
            }
            if (num > 0)
                default_oct = num;
        }
        else if (*p == 'b')
        {
            p += 2;
            int num = 0;
            while (isdigit(*p))
            {
                num = (num * 10) + (*p++ - '0');
            }
            if (num > 0)
                bpm = num;
        }

        if (*p == ',')
            p++;
        else if (*p == ':')
            break;
    }
    if (*p == ':')
        p++;

    // 3. Play Melody Section
    float beatEvery = 60000.0 / bpm; // Base timing from Bluejay

    while (*p != '\0')
    {
        // Parse Duration
        int num = 0;
        while (isdigit(*p))
        {
            num = (num * 10) + (*p++ - '0');
        }
        int noteDuration = (num == 0) ? default_dur : num;

        // Parse Note Letter
        char noteChar = *p;
        if (noteChar >= 'A' && noteChar <= 'Z')
            noteChar += 32; // force lowercase
        if (noteChar == 'h')
            noteChar = 'b'; // Bluejay maps 'h' to 'b'
        if (noteChar != '\0')
            p++;

        // Parse Sharps
        bool sharp = false;
        if (*p == '#')
        {
            sharp = true;
            p++;
        }

        // Parse Dots
        int dots = 0;
        while (*p == '.')
        {
            dots++;
            p++;
        }

        // Parse Octave
        int octave = default_oct;
        if (isdigit(*p))
        {
            octave = *p++ - '0';
        }

        // Sometimes dots appear after the octave in weird RTTTLs
        while (*p == '.')
        {
            dots++;
            p++;
        }

        // Step over comma for the next loop
        if (*p == ',')
            p++;

        // --- BLUEJAY DURATION MATH ---
        float baseDuration = (beatEvery * 4.0) / noteDuration;
        float dotMultiplier = 1.0;
        if (dots == 1)
            dotMultiplier = 1.5;
        else if (dots == 2)
            dotMultiplier = 1.75;
        else if (dots == 3)
            dotMultiplier = 1.875;
        else if (dots == 4)
            dotMultiplier = 1.9375;

        float durationMs = baseDuration * dotMultiplier;

        // --- BLUEJAY FREQUENCY MATH ---
        if (noteChar == 'p')
        {
            _delay_ms(round(durationMs));
        }
        else
        {
            int noteIndex = 0;
            switch (noteChar)
            {
            case 'c':
                noteIndex = 0;
                break;
            case 'd':
                noteIndex = 2;
                break;
            case 'e':
                noteIndex = 4;
                break;
            case 'f':
                noteIndex = 5;
                break;
            case 'g':
                noteIndex = 7;
                break;
            case 'a':
                noteIndex = 9;
                break;
            case 'b':
                noteIndex = 11;
                break;
            }
            if (sharp)
                noteIndex++;

            // Calculate pure frequency
            int semitones = (octave - 4) * 12 + noteIndex;
            float idealFreq = 261.63 * pow(1.059463094, semitones); // Bluejay Twelfth root math

            // Quantize pitch exactly how the ESC hardware does (Temp3 register math)
            long temp3 = round(1000000.0 / (idealFreq * 24.72) - 399.3 / 24.72);
            float bluejayFreq = 1000000.0 / (24.72 * temp3 + 399.3);

            // Play the tone using the original staccato ratio (1.30)
            unsigned long toneDuration = round(durationMs / 1.30);
            unsigned long pauseDuration = round(durationMs) - toneDuration;

            tone(tonePin_Arduino, round(bluejayFreq), 0);
            _delay_ms(toneDuration);
            noTone(tonePin_Arduino);
            _delay_ms(pauseDuration);
        }
    }
}
