#pragma once
#include <TFT_eSPI.h>
#include "../api/WTModels.h"

class BaseScreen {
public:
    virtual void begin(TFT_eSPI* tft)          = 0;
    virtual void onActivate()                   {}   // called when screen becomes visible
    virtual void update(const SharedData& data) = 0; // called by UI task, data already locked
    virtual void draw()                         = 0;
    virtual void onTouch(uint16_t x, uint16_t y) {}
    virtual ~BaseScreen() = default;

protected:
    TFT_eSPI* _tft = nullptr;
};
