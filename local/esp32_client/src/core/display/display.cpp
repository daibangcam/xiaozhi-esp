#include "core/display/display.h"
#include <vector>

TFT_eSPI tft = TFT_eSPI();

void display_init()
{

    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
}

void display_text_center(const String &text, uint8_t size)
{
    tft.setTextSize(size);
    tft.setTextWrap(true);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(text, tft.width() / 2, tft.height() / 2);
}

void display_text_center_line(const String &text, uint32_t y, uint8_t size, uint16_t color)
{
    if (xSemaphoreTake(tft_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
    {
        tft.setTextSize(size);
        tft.setTextDatum(MC_DATUM);
        tft.setTextColor(color, TFT_BLACK);
        tft.drawString(text, tft.width() / 2, y * (size * 8));
        xSemaphoreGive(tft_mutex);
    }
}

void display_clear()
{
    if (xSemaphoreTake(tft_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
    {
        tft.fillScreen(TFT_BLACK);
        xSemaphoreGive(tft_mutex);
    }
}

void display_clear_region(int32_t x, int32_t y, int32_t w, int32_t h)
{
    if (xSemaphoreTake(tft_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
    {
        tft.fillRect(x, y, w, h, TFT_BLACK);
        xSemaphoreGive(tft_mutex);
    }
}
