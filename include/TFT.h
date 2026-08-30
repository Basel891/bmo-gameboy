#pragma once

#include <stdint.h> // Provides standard types like uint8_t and uint16_t
#include <avr/io.h>

// Screen Dimensions
#define TFT_WIDTH 160
#define TFT_HEIGHT 128

#define TFT_CS PB2
#define TFT_RST PB1
#define TFT_DC PB0
#define SWRESET 0x01 // software reset for the lcd
#define SLPOUT 0x11  // sleepout register in the lcd
#define COLMOD 0x3A  // define how to represent colors
#define MADCTL 0x36  // define how to represent the display its self (horizontal or vertical display and RGB or BGR)
#define FRMCTR1 0xB1 // for the fresh rate of the lcd
#define PWCTR1 0xC0  // voltage level setup (define the voltage that makes the transistor on or off)
#define PWCTR2 0xC1  // charge bump boosting (briefly we can say that it boosts  3.3V power supply up to the high voltage rails required )
#define PWCTR3 0xC2  // perational Amplifier Driving Current (Adjusts the bias current of the internal operational amplifiers driving the panel)
#define VMCTR1 0xC5  // important to balance the flow of current in the liquid
#define INVOFF 0x20  // for Normally White Glass
#define INVON 0x21   // Normally black Glass
#define GMCTRP1 0xE0 // positive gamma curve
#define GMCTRN1 0xE1 // negative gamma curve
#define NORON 0x13   // Normal Display Mode On
#define DISPON 0x29  // Main Display ON
#define CASET 0x2A   // column address reset (for the screen)
#define RASET 0x2B   //row address reset (for the screen)
#define RAMWR 0x2C   //Prepare display memory for incoming pixel stream

// Color Definitions
#define TFT_BLACK 0x0000
#define TFT_WHITE 0xFFFF
#define TFT_RED 0xF800

// Public Functions
void TFT_INIT(void);
void TFT_SetAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1); // set the boundaries of the screen
void TFT_DrawPixel(uint8_t x, uint8_t y, uint16_t color) ;

//layer 2
void TFT_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color);   //to make a colorful  rectangle 
void TFT_FillScreen(uint16_t color); 
void TFT_DrawHLine(uint8_t x, uint8_t y, uint8_t w, uint16_t color);          //to make a horizontal line   
void TFT_DrawVLine(uint8_t x, uint8_t y, uint8_t h, uint16_t color);          //to make vertical line         
void TFT_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) ;
void TFT_DrawCircle(uint8_t x0, uint8_t y0, uint8_t r, uint16_t color);
void TFT_FillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint16_t color);
void TFT_DrawChar_Fast(uint8_t x, uint8_t y, char c, uint16_t color, uint16_t bg_color, uint8_t size);
void TFT_DrawString(uint8_t x, uint8_t y, const char *str, uint16_t color, uint16_t bg_color, uint8_t size);
void TFT_DrawBitmap1Bit_Scaled_Clipped(int16_t x, int16_t y, const uint8_t *bitmap, 
                                      uint8_t w, uint8_t h, uint16_t color, 
                                      uint16_t bg_color, uint8_t scale);

     void TFT_DrawImageRGB565_Scaled_Clipped(int16_t x, int16_t y, const uint16_t *image, 
                                        uint8_t w, uint8_t h, uint8_t scale);
                                        
                                        