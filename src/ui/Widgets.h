#pragma once
#include <TFT_eSPI.h>
#include "Theme.h"
#include "../config/Config.h"

namespace Widgets {

// Horizontal progress bar. bgColor fills empty portion.
void drawBar(TFT_eSPI& tft,
             int16_t x, int16_t y, int16_t w, int16_t h,
             float value, float maxValue,
             uint16_t fillColor, uint16_t bgColor = Theme::SURFACE);

// Small label in TEXT_LABEL colour
void drawLabel(TFT_eSPI& tft,
               int16_t x, int16_t y, const char* label,
               uint16_t color = Theme::TEXT_LABEL, uint8_t font = 2);

// Large numeric value with optional unit suffix
void drawBigValue(TFT_eSPI& tft,
                  int16_t x, int16_t y, const char* value,
                  uint16_t color = Theme::TEXT_PRIMARY, uint8_t font = 4);

// Simple compass rose: draws a circle with a heading needle
void drawCompass(TFT_eSPI& tft,
                 int16_t cx, int16_t cy, int16_t r,
                 float headingDeg, uint16_t needleColor = Theme::COL_WARN);

// Gear / flaps state icon (up arrow or down arrow)
void drawStateIcon(TFT_eSPI& tft,
                   int16_t x, int16_t y,
                   bool active, uint16_t activeColor = Theme::COL_WARN);

// Overlay shown when War Thunder is not connected
void drawWaitOverlay(TFT_eSPI& tft, const char* msg = "Waiting for War Thunder...");

// Divider line
void drawDivider(TFT_eSPI& tft,
                 int16_t x, int16_t y, int16_t w,
                 uint16_t color = Theme::BORDER);

} // namespace Widgets
