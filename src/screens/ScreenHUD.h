#pragma once
#include "BaseScreen.h"
#include "../ui/Theme.h"
#include <vector>

class ScreenHUD : public BaseScreen {
public:
    void begin(TFT_eSPI* tft) override;
    void onActivate() override;
    void update(const SharedData& data) override;
    void draw() override;

private:
    HudData  _data;
    bool     _connected = false;
    bool     _dirty     = true;
    size_t   _prevCount = 0;

    static constexpr int MAX_ROWS     = 6;
    static constexpr int ROW_H        = 44;
    static constexpr int HEADER_H     = 20;

    uint16_t messageColor(const String& type) const;
    const char* messagePrefix(const String& type) const;
    String relativeTime(unsigned long ts) const;
};
