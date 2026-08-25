#include "global.h"
#include <avr/io.h>
#include "BIT_MATH.h"
#include "timer.h"
#include "serial.h"
#include <util/delay.h>
#include "controller.h"
#include "SPI.h"
#include "TFT.h"

int main()
{
    SPI_Init();
    TFT_INIT();

    controller_init();
    char* x="basel 3m eyad ";
    
    TFT_DrawString(0,0,x,TFT_RED,TFT_BLACK,1);

    while (true)
    {



    }
}