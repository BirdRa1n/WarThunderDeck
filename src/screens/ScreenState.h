#pragma once
#include "BaseScreen.h"
#include "../ui/Widgets.h"
#include "../ui/Theme.h"

class ScreenState : public BaseScreen {
public:
    void begin(TFT_eSPI* tft) override;
    void onActivate() override;
    void update(const SharedData& data) override;
    void draw() override;

private:
    GameState _data;
    GameState _prev;
    bool      _connected  = false;
    bool      _fullRedraw = true;
    bool      _fireBlink  = false;
    uint32_t  _lastBlink  = 0;

    void drawStatic();
    void drawHealthBar();
    void drawSpeedAlt();
    void drawCompass();
    void drawFireWarning();
};
