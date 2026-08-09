#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h>
#include "timer.h"

// Change to the max desired timer number
#define MAX_TIMERS 4

typedef struct
{
    uint32_t targetTime;
    uint32_t interval;
    void (*callback)(void);
    uint8_t repeat;
    uint8_t active;

} TimerTask;

static volatile uint32_t system_millis = 0;
static TimerTask timerTasks[MAX_TIMERS];

/*
    Interrupt for TIMER0 Output Compare Match A

    Which fires whenever the timer0's counter value (TCNT0) equals the value of the output compare register A (OCR0A)

*/
ISR(TIMER0_COMPA_vect)
{
    system_millis++;
}

void timer_init()
{
    // Set CTC mode (Clear timer on compare)
    TCCR0A = (1 << WGM01);

    // Set prescaler to 64 -> 16Mhz / 64 = 250kHz
    TCCR0B = (1 << CS01) | (1 << CS00);

    // Set compare match value to 1ms: (250kHz * 0.01s) - 1 = 249
    OCR0A = 249;

    // Enable Output Compare Match A Interrupt
    TIMSK0 = (1 << OCIE0A);

    // Enable global interrupts
    sei();
}

uint32_t millis()
{
    uint32_t ms;
    uint8_t sreg = SREG; // Save interrupt state
    cli();               // Disable Interrupts to prevent read corruptions while outputting time
    ms = system_millis;
    SREG = sreg; // Load interrupt state back
    return ms;
}

// Non-repeating timer that calls a callback function after the delay finishes
// Returns timer slot index or -1 if all timer slots are used
// We can use the return id to use the timer_cancel later. Ignore it if you don't want to cancel
int8_t timer_set_timeout(uint32_t delay_ms, void (*cb)(void))
{
    for (uint8_t i = 0; i < MAX_TIMERS; i++)
    {
        if (!timerTasks[i].active)
        {
            timerTasks[i].targetTime = millis() + delay_ms;
            timerTasks[i].interval = 0;
            timerTasks[i].callback = cb;
            timerTasks[i].repeat = 0;
            timerTasks[i].active = 1;
            return i;
        }
    }

    return -1;
}

// Repeating timer
int8_t timer_set_interval(uint32_t delay_ms, void (*cb)(void))
{
    for (uint8_t i = 0; i < MAX_TIMERS; i++)
    {
        if (!timerTasks[i].active)
        {
            timerTasks[i].targetTime = millis() + delay_ms;
            timerTasks[i].interval = 0;
            timerTasks[i].callback = cb;
            timerTasks[i].repeat = 1;
            timerTasks[i].active = 1;
            return i;
        }
    }

    return -1;
}

// Pass the timer id that you got from the return of the timer_set_X to cancel the timer at any time before cb execution
void timer_cancel(uint8_t timer_id)
{
    if (timer_id < MAX_TIMERS)
        timerTasks[timer_id].active = 0;
}

// PUT IT INSIDE THE WHILE(1) LOOP
// Updates the state of each timers and handles everything
void timer_update()
{
    uint32_t current_ms = millis();

    for (uint8_t i = 0; i < MAX_TIMERS; i++)
    {
        if (timerTasks[i].active && (current_ms >= timerTasks[i].targetTime))
        {
            if (timerTasks[i].callback != NULL)
                timerTasks[i].callback();

            if (timerTasks[i].repeat)
                timerTasks[i].targetTime = current_ms + timerTasks[i].interval;
            else
                timerTasks[i].active = 0;
        }
    }
}
