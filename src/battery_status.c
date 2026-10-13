#include "battery_status.h"
#include "adc.h"
#include "TFT.h"
#include <stdio.h>
#include "global.h"
#include "menu.h"

// Encapsulated module state
static uint32_t last_adc_time = 0;
static uint8_t prev_battery_percent = 255;
static uint16_t prev_milli_volt = 0;
static uint8_t force_redraw_flag = true;

#define TEXT_SHIFT 35

// Private helper to draw the header UI
static void render_battery_status(uint16_t milli_volt, uint8_t percentage)
{
    char buf[10];

    // Clear previous header text area
    TFT_FillRect(0, 2, TFT_WIDTH, 12, MENU_BACKGROUND);

    // Battery Icon (Y: 4 to 12)
    TFT_FillRect(104 + TEXT_SHIFT, 4, 18, 9, TFT_WHITE);
    TFT_FillRect(105 + TEXT_SHIFT, 5, 16, 7, MENU_BACKGROUND);
    TFT_FillRect(122 + TEXT_SHIFT, 6, 2, 5, TFT_WHITE);

    uint8_t bar_width = (percentage * 14) / 100;
    if (bar_width > 0)
    {
        uint16_t bar_color = (percentage > 20) ? TFT_GREEN : TFT_RED;
        TFT_FillRect(106 + TEXT_SHIFT, 6, bar_width, 5, bar_color);
    }

    // Centered Percentage (Y = 4)
    sprintf(buf, "%3u%%", percentage);
    TFT_DrawString(74 + TEXT_SHIFT, 4, buf, TFT_WHITE, MENU_BACKGROUND, 1);

    // Centered Voltage (Y = 4)
    sprintf(buf, "%u.%02uV", milli_volt / 1000, (milli_volt % 1000) / 10);
    TFT_DrawString(2, 4, buf, TFT_WHITE, MENU_BACKGROUND, 1);

    // Separator line
    TFT_DrawHLine_Fast(0, 16, TFT_WIDTH, TFT_WHITE);
}

void battery_init(void)
{
    adc_init();
    last_adc_time = 0;
    prev_battery_percent = 255;
    prev_milli_volt = 0;
    force_redraw_flag = true;
}

void battery_force_redraw(void)
{
    force_redraw_flag = true;
}

void battery_update(uint32_t current_time)
{
    if (force_redraw_flag)
    {
        uint16_t mv = battery_get_mv();
        uint8_t pct = battery_get_percentage(mv);

        render_battery_status(mv, pct);

        prev_milli_volt = mv;
        prev_battery_percent = pct;
        last_adc_time = current_time;
        force_redraw_flag = false;
        return;
    }

    // Non-blocking 2-second check
    if (current_time - last_adc_time >= 2000)
    {
        last_adc_time = current_time;

        uint16_t milli_volt = battery_get_mv();
        uint8_t battery_percent = battery_get_percentage(milli_volt);

        int16_t mv_diff = (int16_t)milli_volt - (int16_t)prev_milli_volt;
        if (mv_diff < 0)
            mv_diff = -mv_diff;

        if (battery_percent != prev_battery_percent || mv_diff >= 50)
        {
            render_battery_status(milli_volt, battery_percent);
            prev_battery_percent = battery_percent;
            prev_milli_volt = milli_volt;
        }
    }
}