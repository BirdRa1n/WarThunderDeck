#include "Widgets.h"
#include <cmath>

namespace Widgets {

void drawBar(TFT_eSPI& tft,
             int16_t x, int16_t y, int16_t w, int16_t h,
             float value, float maxValue,
             uint16_t fillColor, uint16_t bgColor) {
    if (maxValue <= 0.f) maxValue = 1.f;
    float ratio = constrain(value / maxValue, 0.f, 1.f);
    int16_t filled = (int16_t)(w * ratio);

    tft.fillRect(x, y, filled, h, fillColor);
    if (filled < w) {
        tft.fillRect(x + filled, y, w - filled, h, bgColor);
    }
    tft.drawRect(x, y, w, h, Theme::BORDER);
}

void drawLabel(TFT_eSPI& tft,
               int16_t x, int16_t y, const char* label,
               uint16_t color, uint8_t font) {
    tft.setTextColor(color, Theme::BG);
    tft.setTextFont(font);
    tft.setCursor(x, y);
    tft.print(label);
}

void drawBigValue(TFT_eSPI& tft,
                  int16_t x, int16_t y, const char* value,
                  uint16_t color, uint8_t font) {
    tft.setTextColor(color, Theme::BG);
    tft.setTextFont(font);
    tft.setCursor(x, y);
    tft.print(value);
}

void drawCompass(TFT_eSPI& tft,
                 int16_t cx, int16_t cy, int16_t r,
                 float headingDeg, uint16_t needleColor) {
    tft.drawCircle(cx, cy, r, Theme::BORDER);
    tft.drawCircle(cx, cy, r - 1, Theme::SURFACE);

    // Cardinal labels
    tft.setTextColor(Theme::TEXT_DIM, Theme::BG);
    tft.setTextFont(1);
    tft.setCursor(cx - 3, cy - r + 2);  tft.print("N");
    tft.setCursor(cx - 3, cy + r - 10); tft.print("S");
    tft.setCursor(cx + r - 8, cy - 4);  tft.print("E");
    tft.setCursor(cx - r + 2, cy - 4);  tft.print("W");

    // Needle
    float rad = headingDeg * (float)M_PI / 180.f;
    int16_t nx = cx + (int16_t)((r - 4) * sinf(rad));
    int16_t ny = cy - (int16_t)((r - 4) * cosf(rad));
    tft.drawLine(cx, cy, nx, ny, needleColor);

    // Centre dot
    tft.fillCircle(cx, cy, 2, Theme::TEXT_DIM);
}

void drawStateIcon(TFT_eSPI& tft,
                   int16_t x, int16_t y,
                   bool active, uint16_t activeColor) {
    uint16_t col = active ? activeColor : Theme::TEXT_DIM;
    if (active) {
        // Down arrow (gear down / flaps extended)
        tft.fillTriangle(x, y, x + 8, y, x + 4, y + 8, col);
    } else {
        // Up arrow (retracted)
        tft.fillTriangle(x, y + 8, x + 8, y + 8, x + 4, y, col);
    }
}

void drawWaitOverlay(TFT_eSPI& tft, const char* msg) {
    tft.fillRect(10, CONTENT_H / 2 - 20, SCREEN_W - 20, 40, Theme::SURFACE2);
    tft.drawRect(10, CONTENT_H / 2 - 20, SCREEN_W - 20, 40, Theme::BORDER);
    tft.setTextColor(Theme::COL_WARN, Theme::SURFACE2);
    tft.setTextFont(2);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(msg, SCREEN_W / 2, CONTENT_H / 2);
    tft.setTextDatum(TL_DATUM);
}

void drawDivider(TFT_eSPI& tft,
                 int16_t x, int16_t y, int16_t w,
                 uint16_t color) {
    tft.drawFastHLine(x, y, w, color);
}

} // namespace Widgets
