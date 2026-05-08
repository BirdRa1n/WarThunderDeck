#include "ScreenManager.h"
#include "../ui/Theme.h"
#include <WiFi.h>

const char* ScreenManager::TAB_LABELS[SCREEN_COUNT] = {
    "INST", "MAP", "STATE", "HUD"
};

ScreenManager::ScreenManager(UserConfig& cfg) : _cfg(cfg) {
    _setup = new ScreenSetup(cfg);
    _screens[SCREEN_INDICATORS] = new ScreenIndicators();
    _screens[SCREEN_MAP]        = new ScreenMap();
    _screens[SCREEN_STATE]      = new ScreenState();
    _screens[SCREEN_HUD]        = new ScreenHUD();
}

void ScreenManager::begin(TFT_eSPI* tft) {
    _tft = tft;
    _setup->begin(tft);
    for (int i = 0; i < SCREEN_COUNT; i++) {
        _screens[i]->begin(tft);
    }

    _tft->fillScreen(Theme::BG);

    // If we already have a configured IP, skip setup
    if (!_cfg.pcIP.isEmpty() && _cfg.pcIP != "192.168.1.100" &&
        WiFi.status() == WL_CONNECTED) {
        _setupMode = false;
        _active    = _cfg.defaultScreen;
        _screens[_active]->onActivate();
    } else {
        _setupMode = true;
        _setup->onActivate();
        _setup->draw();
    }
}

void ScreenManager::update(SharedData& data, SemaphoreHandle_t mutex) {
    if (_setupMode) {
        if (_setup->isReady()) {
            _setupMode = false;
            _active    = _cfg.defaultScreen;
            _tft->fillScreen(Theme::BG);
            _screens[_active]->onActivate();
            _tabDirty = true;
        }
        return;
    }

    SharedData copy;
    if (xSemaphoreTake(mutex, pdMS_TO_TICKS(30)) == pdTRUE) {
        copy = data;
        xSemaphoreGive(mutex);
    }
    _localCopy = copy;
    _screens[_active]->update(_localCopy);
}

void ScreenManager::draw() {
    if (_setupMode) {
        _setup->draw();
        return;
    }

    _screens[_active]->draw();
    drawTabBar();
}

void ScreenManager::handleTouch(uint16_t x, uint16_t y) {
    if (_setupMode) {
        _setup->onTouch(x, y);
        return;
    }

    // Tab bar region
    if (y >= CONTENT_H) {
        int tabIdx = x / TAB_W;
        if (tabIdx >= 0 && tabIdx < SCREEN_COUNT && tabIdx != _active) {
            switchTo(tabIdx);
        }
        return;
    }

    _screens[_active]->onTouch(x, y);
}

void ScreenManager::switchTo(int index) {
    if (index < 0 || index >= SCREEN_COUNT) return;
    _active = index;
    _tft->fillRect(0, 0, SCREEN_W, CONTENT_H, Theme::BG);
    _screens[_active]->onActivate();
    _tabDirty = true;
}

void ScreenManager::drawTabBar() {
    if (!_tabDirty) return;
    _tabDirty = false;

    for (int i = 0; i < SCREEN_COUNT; i++) {
        int16_t tx = i * TAB_W;
        bool    active = (i == _active);

        _tft->fillRect(tx, CONTENT_H, TAB_W, TAB_BAR_H,
                       active ? Theme::TAB_ACTIVE_BG : Theme::TAB_BG);

        _tft->setTextColor(active ? Theme::TAB_ACTIVE_TEXT : Theme::TAB_TEXT,
                           active ? Theme::TAB_ACTIVE_BG  : Theme::TAB_BG);
        _tft->setTextFont(1);
        _tft->setTextDatum(MC_DATUM);
        _tft->drawString(TAB_LABELS[i], tx + TAB_W / 2, CONTENT_H + TAB_BAR_H / 2);
    }
    _tft->setTextDatum(TL_DATUM);
}
