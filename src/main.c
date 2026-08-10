#include "global.h"
#include <avr/io.h>
#include "BIT_MATH.h"
#include "timer.h"
#include <util/delay.h>

void led()
{
    TOG_BIT(PORTC, 0);
}

void turnoff(void *arg)
{
    uint8_t timer_id = (uint8_t)arg;
    timer_cancel(timer_id);
}

int main()
{
    SET_BIT(DDRC, 0);

    CLR_BIT(PORTC, 0);

    timer_init();

    uint8_t timer_id = timer_set_interval(1000, led);

    while (true)
    {
        timer_update();

        timer_set_timeout_arg(5000, turnoff, (void *)timer_id);
    }
}