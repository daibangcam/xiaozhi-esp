#pragma once
#include <TFT_eSPI.h>
#include "setup.h"

extern TFT_eSPI tft;

void display_init();
void display_text_center_line(const String &text, uint32_t y, uint8_t size, uint16_t color);
void display_clear();
void display_text_center(const String &text, uint8_t size);
void display_clear_region(int32_t x, int32_t y, int32_t w, int32_t h);