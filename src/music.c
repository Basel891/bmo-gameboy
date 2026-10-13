/*
This code is based on bluejay's RTTL parser rewritten in c instead of javascript.

This version is non blocking and uses hardware timers to calculate all the timings for the notes.

*/

#include <math.h>

#include "music.h"
#include "tone.h"

#define isdigit(n) (n >= '0' && n <= '9')

// --- State Machine Variables ---
static const char *current_p = NULL;
static uint16_t bpm = 63;
static uint8_t default_oct = 6;
static uint8_t default_dur = 4;
static float beatEvery = 0;

static bool is_playing = false;
static bool is_note_sounding = false;
static unsigned long note_start_time = 0;
static unsigned long current_phase_duration = 0;
static unsigned long next_pause_duration = 0;

static const char *original_song_ptr = NULL;

void start_song(const char *p)
{
    // Crucial Guard
    if (!p || !strchr(p, ':'))
        return;

    original_song_ptr = p;

    noTone(tonePin_Arduino);

    // Reset state machine
    is_playing = true;
    is_note_sounding = false;
    current_phase_duration = 0; // Force first note to play immediately
    note_start_time = millis();

    // Default RTTTL values
    bpm = 63;
    default_oct = 6;
    default_dur = 4;

    current_p = p;

    // 1. Skip Name Section
    while (*current_p != ':' && *current_p != '\0')
        current_p++;
    if (*current_p == ':')
        current_p++;

    // 2. Parse Defaults Section (Bluejay style)
    while (*current_p != ':' && *current_p != '\0')
    {
        if (*current_p == 'd')
        {
            current_p += 2;
            int num = 0;
            while (isdigit(*current_p))
            {
                num = (num * 10) + (*current_p++ - '0');
            }
            if (num > 0)
                default_dur = num;
        }
        else if (*current_p == 'o')
        {
            current_p += 2;
            int num = 0;
            while (isdigit(*current_p))
            {
                num = (num * 10) + (*current_p++ - '0');
            }
            if (num > 0)
                default_oct = num;
        }
        else if (*current_p == 'b')
        {
            current_p += 2;
            int num = 0;
            while (isdigit(*current_p))
            {
                num = (num * 10) + (*current_p++ - '0');
            }
            if (num > 0)
                bpm = num;
        }

        if (*current_p == ',')
            current_p++;
        else if (*current_p == ':')
            break;
    }
    if (*current_p == ':')
        current_p++;

    // Calculate base timing
    beatEvery = 60000.0 / bpm;
}

void update_music(void)
{
    // If no song is active, instantly return to the main loop
    if (!is_playing)
        return;

    unsigned long current_time = millis();

    // Has the current phase (playing a tone OR resting) elapsed?
    if (current_time - note_start_time >= current_phase_duration)
    {
        // ---------------------------------------------------------
        // STATE 1: We just finished playing a tone. Start the pause.
        // ---------------------------------------------------------
        if (is_note_sounding)
        {
            noTone(tonePin_Arduino);
            is_note_sounding = false;
            current_phase_duration = next_pause_duration; // Staccato gap
            note_start_time = current_time;
            return; // Exit and wait for the pause gap to finish
        }

        // ---------------------------------------------------------
        // STATE 2: We just finished a pause. Parse the next note!
        // ---------------------------------------------------------

        // Are we at the end of the song?
        if (*current_p == '\0')
        {
            start_song(original_song_ptr);
            return;
        }

        // Parse Duration
        int num = 0;
        while (isdigit(*current_p))
        {
            num = (num * 10) + (*current_p++ - '0');
        }
        int noteDuration = (num == 0) ? default_dur : num;

        // Parse Note Letter
        char noteChar = *current_p;
        if (noteChar >= 'A' && noteChar <= 'Z')
            noteChar += 32; // force lowercase
        if (noteChar == 'h')
            noteChar = 'b'; // Bluejay maps 'h' to 'b'
        if (noteChar != '\0')
            current_p++;

        // Parse Sharps
        bool sharp = false;
        if (*current_p == '#')
        {
            sharp = true;
            current_p++;
        }

        // Parse Dots
        int dots = 0;
        while (*current_p == '.')
        {
            dots++;
            current_p++;
        }

        // Parse Octave
        int octave = default_oct;
        if (isdigit(*current_p))
        {
            octave = *current_p++ - '0';
        }

        // Sometimes dots appear after the octave in weird RTTTLs
        while (*current_p == '.')
        {
            dots++;
            current_p++;
        }

        // Step over comma for the next loop iteration
        if (*current_p == ',')
            current_p++;

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

        // --- BLUEJAY FREQUENCY MATH & EXECUTION ---
        if (noteChar == 'p')
        {
            is_note_sounding = false;
            current_phase_duration = round(durationMs);
            note_start_time = current_time;
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
            float idealFreq = 261.63 * pow(1.059463094, semitones);

            // Quantize pitch exactly how the ESC hardware does
            long temp3 = round(1000000.0 / (idealFreq * 24.72) - 399.3 / 24.72);
            float bluejayFreq = 1000000.0 / (24.72 * temp3 + 399.3);

            // Calculate staccato timings
            unsigned long toneDuration = round(durationMs / 1.30);
            next_pause_duration = round(durationMs) - toneDuration;

            // Trigger the hardware timer tone!
            tone(tonePin_Arduino, round(bluejayFreq), 0);

            // Advance state machine variables
            is_note_sounding = true;
            current_phase_duration = toneDuration;
            note_start_time = current_time;
        }
    }
}