#include "TouchHandler.h"

void TouchHandler::begin(SPIClass& spi) {
    _ts = new XPT2046_Touchscreen(PIN_TOUCH_CS, PIN_TOUCH_IRQ);
    _ts->begin(spi);
    _ts->setRotation(0);  // portrait
}

bool TouchHandler::getTouch(uint16_t& sx, uint16_t& sy) {
    if (!_ts || !_ts->tirqTouched() || !_ts->touched()) return false;

    unsigned long now = millis();
    if (now - _lastTouch < DEBOUNCE_MS) return false;

    TS_Point p = _ts->getPoint();
    if (p.z < 300) return false;  // too light

    mapToScreen((uint16_t)p.x, (uint16_t)p.y, sx, sy);
    _lastTouch = now;
    return true;
}

void TouchHandler::mapToScreen(uint16_t rawX, uint16_t rawY, uint16_t& sx, uint16_t& sy) {
    // Map raw ADC values to screen coordinates (portrait orientation)
    sx = (uint16_t)map(rawX, TS_MINX, TS_MAXX, 0, SCREEN_W - 1);
    sy = (uint16_t)map(rawY, TS_MINY, TS_MAXY, 0, SCREEN_H - 1);
    sx = constrain(sx, 0, SCREEN_W - 1);
    sy = constrain(sy, 0, SCREEN_H - 1);
}
