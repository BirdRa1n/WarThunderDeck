#pragma once
#include <TFT_eSPI.h>
#include <freertos/semphr.h>
#include "BaseScreen.h"
#include "ScreenSetup.h"
#include "ScreenIndicators.h"
#include "ScreenMap.h"
#include "ScreenState.h"
#include "ScreenHUD.h"
#include "../config/UserConfig.h"
#include "../ui/Theme.h"

class ScreenManager {
public:
    explicit ScreenManager(UserConfig& cfg);

    void begin(TFT_eSPI* tft);
    void update(SharedData& data, SemaphoreHandle_t mutex);
    void draw();
    void handleTouch(uint16_t x, uint16_t y);

    bool isSetupMode() const { return _setupMode; }

private:
    UserConfig&      _cfg;
    TFT_eSPI*        _tft        = nullptr;
    int              _active     = SCREEN_INDICATORS;
    bool             _setupMode  = true;
    bool             _tabDirty   = true;

    ScreenSetup*     _setup      = nullptr;
    BaseScreen*      _screens[SCREEN_COUNT] = {};

    SharedData       _localCopy;

    void switchTo(int index);
    void drawTabBar();

    static constexpr int TAB_W = SCREEN_W / SCREEN_COUNT;
    static const char*   TAB_LABELS[SCREEN_COUNT];
};
