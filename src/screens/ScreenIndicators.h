#pragma once
#include "BaseScreen.h"
#include "../ui/Widgets.h"
#include "../ui/Theme.h"

class ScreenIndicators : public BaseScreen {
public:
    void begin(TFT_eSPI* tft) override;
    void onActivate() override;
    void update(const SharedData& data) override;
    void draw() override;

private:
    IndicatorData _data;
    IndicatorData _prev;
    bool          _connected   = false;
    bool          _fullRedraw  = true;
    uint8_t       _blinkCount  = 0;

    void drawStatic();
    void drawDynamic();
    void drawVehicleName();
    void drawSpeedAlt();
    void drawThrottleBar();
    void drawFuelBar();
    void drawRpmMach();
    void drawGForce();
    void drawGearFlaps();
};
