#include "global.h"
#include <avr/io.h>
#include "BIT_MATH.h"
#include "timer.h"
#include "serial.h"
#include <util/delay.h>
#include "controller.h"

int main()
{
    controller_init();
    serial_init(9600);

    while (true)
    {
        _delay_ms(200);
        if (get_key(RIGHT))
            serial_print("RIGHT");
        if (get_key(LEFT))
            serial_print("LEFT");
        if (get_key(UP))
            serial_print("UP");
        if (get_key(DOWN))
            serial_print("DOWN");
        if (get_key(START))
            serial_print("START");
        if (get_key(BACK))
            serial_print("BACK");
    }
}