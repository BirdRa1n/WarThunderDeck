#include "ScreenState.h"
#include "../ui/Widgets.h"
#include "../config/Config.h"

// ─── Layout constants ────────────────────────────────────────────────────────
static constexpr int16_t Y_HEADER    =  2;
static constexpr int16_t Y_HP_LABEL  = 18;
static constexpr int16_t Y_HP_BAR    = 32;
static constexpr int16_t Y_SPEED_L   = 60;
static constexpr int16_t Y_SPEED_V   = 74;
static constexpr int16_t Y_HDG_L     = 60;
static constexpr int16_t Y_HDG_V     = 74;
static constexpr int16_t Y_COMPASS   = 185;
static constexpr int16_t COMPASS_R   = 50;
static constexpr int16_t BAR_X       =  4;
static constexpr int16_t BAR_W       = SCREEN_W - 8;
static constexpr int16_t HP_BAR_H    = 20;

void ScreenState::begin(TFT_eSPI* tft) {
    _tft = tft;
}

void ScreenState::onActivate() {
    _fullRedraw = true;
}

void ScreenState::update(const SharedData& d) {
    _prev      = _data;
    _data      = d.state;
    _connected = d.wtConnected;
}

void ScreenState::draw() {
    if (!_connected || !_data.valid) {
        if (_fullRedraw) {
            _tft->fillRect(0, 0, SCREEN_W, CONTENT_H, Theme::BG);
            _fullRedraw = false;
        }
        Widgets::drawWaitOverlay(*_tft);
        return;
    }

    if (_fullRedraw) {
        _tft->fillRect(0, 0, SCREEN_W, CONTENT_H, Theme::BG);
        drawStatic();
        _fullRedraw = false;
    }

    // Fire-warning blink (background flash)
    if (_data.fire) {
        uint32_t now = millis();
        if (now - _lastBlink > 400) {
            _fireBlink = !_fireBlink;
            _lastBlink = now;
            _tft->fillRect(0, 0, SCREEN_W, CONTENT_H, _fireBlink ? 0x4000 : Theme::BG);
            _fullRedraw = true; // redraw everything on next cycle
            return;
        }
    } else {
        _fireBlink = false;
    }

    drawHealthBar();
    drawSpeedAlt();
    drawCompass();
    drawFireWarning();
}

void ScreenState::drawStatic() {
    _tft->setTextColor(Theme::COL_CYAN, Theme::BG);
    _tft->setTextFont(2);
    _tft->setTextDatum(MC_DATUM);
    _tft->drawString("BATTLE STATE", SCREEN_W / 2, Y_HEADER + 6);
    _tft->setTextDatum(TL_DATUM);

    Widgets::drawDivider(*_tft, 0, Y_HP_LABEL - 2, SCREEN_W);
    Widgets::drawLabel(*_tft, 4, Y_HP_LABEL, "HEALTH");

    Widgets::drawDivider(*_tft, 0, Y_SPEED_L - 4, SCREEN_W);
    Widgets::drawDivider(*_tft, SCREEN_W / 2, Y_SPEED_L, 1);

    Widgets::drawLabel(*_tft, 4,                 Y_SPEED_L, "SPD km/h");
    Widgets::drawLabel(*_tft, SCREEN_W / 2 + 4, Y_SPEED_L, "ALT m");

    Widgets::drawDivider(*_tft, 0, Y_SPEED_V + 30, SCREEN_W);
}

void ScreenState::drawHealthBar() {
    uint16_t col = Theme::healthColor(_data.health);
    Widgets::drawBar(*_tft, BAR_X, Y_HP_BAR, BAR_W, HP_BAR_H,
                    _data.health, 100.f, col);

    char buf[8];
    snprintf(buf, sizeof(buf), "%.0f%%", _data.health);
    _tft->setTextColor(Theme::BG, col);
    _tft->setTextFont(2);
    _tft->setTextDatum(MC_DATUM);
    _tft->drawString(buf, SCREEN_W / 2, Y_HP_BAR + HP_BAR_H / 2);
    _tft->setTextDatum(TL_DATUM);
}

void ScreenState::drawSpeedAlt() {
    char buf[12];

    snprintf(buf, sizeof(buf), "%.0f", _data.speed);
    _tft->fillRect(0, Y_SPEED_V, SCREEN_W / 2 - 1, 28, Theme::BG);
    Widgets::drawBigValue(*_tft, 4, Y_SPEED_V, buf, Theme::TEXT_PRIMARY, 4);

    snprintf(buf, sizeof(buf), "%.0f", _data.altitude);
    _tft->fillRect(SCREEN_W / 2 + 1, Y_HDG_V, SCREEN_W / 2 - 1, 28, Theme::BG);
    Widgets::drawBigValue(*_tft, SCREEN_W / 2 + 4, Y_HDG_V, buf, Theme::TEXT_PRIMARY, 4);
}

void ScreenState::drawCompass() {
    _tft->fillCircle(SCREEN_W / 2, Y_COMPASS, COMPASS_R + 2, Theme::BG);
    Widgets::drawCompass(*_tft, SCREEN_W / 2, Y_COMPASS, COMPASS_R, _data.heading);

    char buf[8];
    snprintf(buf, sizeof(buf), "%.0f\xb0", _data.heading);
    _tft->setTextColor(Theme::COL_WARN, Theme::BG);
    _tft->setTextFont(2);
    _tft->setTextDatum(MC_DATUM);
    _tft->drawString(buf, SCREEN_W / 2, Y_COMPASS + COMPASS_R + 8);
    _tft->setTextDatum(TL_DATUM);
}

void ScreenState::drawFireWarning() {
    if (_data.fire) {
        _tft->setTextColor(Theme::COL_DANGER, Theme::BG);
        _tft->setTextFont(4);
        _tft->setTextDatum(MC_DATUM);
        _tft->drawString("!! FIRE !!", SCREEN_W / 2, CONTENT_H - 20);
        _tft->setTextDatum(TL_DATUM);
    } else {
        _tft->fillRect(0, CONTENT_H - 36, SCREEN_W, 36, Theme::BG);
    }
}
