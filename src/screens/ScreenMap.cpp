#include "ScreenMap.h"
#include "../ui/Widgets.h"

void ScreenMap::begin(TFT_eSPI* tft) {
    _tft = tft;
}

void ScreenMap::onActivate() {
    _fullRedraw = true;
}

void ScreenMap::update(const SharedData& d) {
    _data      = d.map;
    _connected = d.wtConnected;
}

void ScreenMap::draw() {
    if (!_connected || !_data.valid) {
        if (_fullRedraw) {
            _tft->fillRect(0, 0, SCREEN_W, CONTENT_H, Theme::BG);
            _fullRedraw = false;
        }
        Widgets::drawWaitOverlay(*_tft);
        return;
    }

    if (_fullRedraw || _data.mapName != _prevMapName) {
        drawBackground();
        drawMapName();
        _prevMapName = _data.mapName;
        _fullRedraw  = false;
    } else {
        // Erase previous objects
        _tft->fillRect(MAP_LEFT, MAP_TOP, MAP_W, MAP_H, Theme::MAP_BG);
    }

    drawObjects();
}

void ScreenMap::drawBackground() {
    _tft->fillRect(0, 0, SCREEN_W, CONTENT_H, Theme::BG);
    _tft->fillRect(MAP_LEFT, MAP_TOP, MAP_W, MAP_H, Theme::MAP_BG);
    _tft->drawRect(MAP_LEFT, MAP_TOP, MAP_W, MAP_H, Theme::BORDER);

    // Faint grid
    int gridStep = MAP_W / 4;
    for (int i = 1; i < 4; i++) {
        _tft->drawFastVLine(MAP_LEFT + i * gridStep, MAP_TOP, MAP_H, Theme::MAP_GRID);
        _tft->drawFastHLine(MAP_LEFT, MAP_TOP + i * (MAP_H / 4), MAP_W, Theme::MAP_GRID);
    }
}

void ScreenMap::drawMapName() {
    _tft->fillRect(0, 0, SCREEN_W, MAP_TOP, Theme::BG);
    _tft->setTextColor(Theme::TEXT_DIM, Theme::BG);
    _tft->setTextFont(1);
    _tft->setTextDatum(MC_DATUM);

    String name = _data.mapName;
    int slash = name.lastIndexOf('/');
    if (slash >= 0) name = name.substring(slash + 1);
    _tft->drawString(name.c_str(), SCREEN_W / 2, MAP_TOP / 2);
    _tft->setTextDatum(TL_DATUM);
}

void ScreenMap::drawObjects() {
    bool playerDrawn = false;

    for (const auto& obj : _data.objects) {
        int16_t sx = mapX(obj.x);
        int16_t sy = mapY(obj.y);

        // Clamp to map area
        sx = constrain(sx, MAP_LEFT + 2, MAP_RIGHT - 2);
        sy = constrain(sy, MAP_TOP  + 2, MAP_BOTTOM - 2);

        bool isPlayer = (obj.icon == "Player");
        uint16_t col  = Theme::mapObjectColor(obj.color.c_str());

        if (isPlayer) {
            // Larger marker for the player
            _tft->fillCircle(sx, sy, 4, col);
            _tft->drawCircle(sx, sy, 5, Theme::TEXT_PRIMARY);
            playerDrawn = true;
        } else {
            _tft->fillCircle(sx, sy, 2, col);
        }
    }

    // Legend in top-right corner of map area
    int16_t lx = MAP_RIGHT - 32;
    int16_t ly = MAP_TOP + 4;
    _tft->fillCircle(lx,      ly,     3, Theme::MAP_PLAYER);
    _tft->fillCircle(lx,      ly + 8, 2, Theme::MAP_ALLY);
    _tft->fillCircle(lx,      ly + 16, 2, Theme::MAP_ENEMY);
    _tft->setTextFont(1);
    _tft->setTextColor(Theme::TEXT_DIM, Theme::MAP_BG);
    _tft->setCursor(lx + 6, ly - 3);  _tft->print("You");
    _tft->setCursor(lx + 6, ly + 5);  _tft->print("Ally");
    _tft->setCursor(lx + 6, ly + 13); _tft->print("Foe");
    (void)playerDrawn;
}

int16_t ScreenMap::mapX(float nx) const {
    return MAP_LEFT + (int16_t)(nx * MAP_W);
}

int16_t ScreenMap::mapY(float ny) const {
    return MAP_TOP + (int16_t)(ny * MAP_H);
}
