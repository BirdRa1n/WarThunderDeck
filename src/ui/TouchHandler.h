#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include "../config/Config.h"

class TouchHandler {
public:
    void begin(SPIClass& spi);

    // Returns true if a debounced touch event is available.
    // Fills sx/sy with screen-space pixel coordinates (portrait 240×320).
    bool getTouch(uint16_t& sx, uint16_t& sy);

private:
    XPT2046_Touchscreen* _ts = nullptr;
    unsigned long _lastTouch  = 0;
    static constexpr uint32_t DEBOUNCE_MS = 120;

    void mapToScreen(uint16_t rawX, uint16_t rawY, uint16_t& sx, uint16_t& sy);
};
