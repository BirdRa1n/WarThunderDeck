#pragma once
#include "BaseScreen.h"
#include "../ui/Theme.h"
#include "../config/Config.h"

class ScreenMap : public BaseScreen {
public:
    void begin(TFT_eSPI* tft) override;
    void onActivate() override;
    void update(const SharedData& data) override;
    void draw() override;

private:
    MapData  _data;
    bool     _connected  = false;
    bool     _fullRedraw = true;
    String   _prevMapName;

    static constexpr int MAP_MARGIN = 20;
    static constexpr int MAP_TOP    = 20;   // below vehicle name bar
    static constexpr int MAP_BOTTOM = CONTENT_H - 5;
    static constexpr int MAP_LEFT   = MAP_MARGIN;
    static constexpr int MAP_RIGHT  = SCREEN_W - MAP_MARGIN;
    static constexpr int MAP_W      = MAP_RIGHT - MAP_LEFT;
    static constexpr int MAP_H      = MAP_BOTTOM - MAP_TOP;

    void drawBackground();
    void drawObjects();
    void drawMapName();
    int16_t mapX(float nx) const;
    int16_t mapY(float ny) const;
};
