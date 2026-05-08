#include "ScreenHUD.h"
#include "../ui/Widgets.h"
#include "../config/Config.h"

void ScreenHUD::begin(TFT_eSPI* tft) {
    _tft = tft;
}

void ScreenHUD::onActivate() {
    _dirty = true;
}

void ScreenHUD::update(const SharedData& d) {
    bool changed = (d.hud.messages.size() != _prevCount ||
                    d.hud.lastUpdate != _data.lastUpdate);
    _data      = d.hud;
    _connected = d.wtConnected;
    if (changed) _dirty = true;
    _prevCount = d.hud.messages.size();
}

void ScreenHUD::draw() {
    if (!_dirty) return;
    _dirty = false;

    _tft->fillRect(0, 0, SCREEN_W, CONTENT_H, Theme::BG);

    // Header
    _tft->setTextColor(Theme::COL_CYAN, Theme::BG);
    _tft->setTextFont(2);
    _tft->setTextDatum(MC_DATUM);
    _tft->drawString("HUD ALERTS", SCREEN_W / 2, HEADER_H / 2 + 2);
    _tft->setTextDatum(TL_DATUM);
    Widgets::drawDivider(*_tft, 0, HEADER_H, SCREEN_W);

    if (!_connected) {
        Widgets::drawWaitOverlay(*_tft);
        return;
    }

    if (_data.messages.empty()) {
        _tft->setTextColor(Theme::TEXT_DIM, Theme::BG);
        _tft->setTextFont(2);
        _tft->setTextDatum(MC_DATUM);
        _tft->drawString("No events yet.", SCREEN_W / 2, CONTENT_H / 2);
        _tft->setTextDatum(TL_DATUM);
        return;
    }

    int count = min((int)_data.messages.size(), MAX_ROWS);
    for (int i = 0; i < count; i++) {
        const HudMessage& m = _data.messages[i];
        int16_t rowY = HEADER_H + 2 + i * ROW_H;

        // Row background
        uint16_t rowBg = (i % 2 == 0) ? Theme::SURFACE : Theme::BG;
        _tft->fillRect(0, rowY, SCREEN_W, ROW_H - 2, rowBg);

        // Prefix tag
        uint16_t col = messageColor(m.type);
        const char* prefix = messagePrefix(m.type);
        _tft->fillRect(0, rowY, 4, ROW_H - 2, col);

        // Type badge
        _tft->setTextColor(col, rowBg);
        _tft->setTextFont(1);
        _tft->setCursor(8, rowY + 3);
        _tft->print(prefix);

        // Message text (clipped to screen width)
        _tft->setTextColor(Theme::TEXT_PRIMARY, rowBg);
        _tft->setTextFont(2);
        _tft->setCursor(8, rowY + 14);
        String txt = m.text;
        if (txt.length() > 26) txt = txt.substring(0, 25) + "\x7E";
        _tft->print(txt.c_str());

        // Relative time
        String rel = relativeTime(m.timestamp);
        _tft->setTextColor(Theme::TEXT_DIM, rowBg);
        _tft->setTextFont(1);
        _tft->setTextDatum(TR_DATUM);
        _tft->drawString(rel.c_str(), SCREEN_W - 2, rowY + 3);
        _tft->setTextDatum(TL_DATUM);
    }
}

uint16_t ScreenHUD::messageColor(const String& type) const {
    if (type == "kill")   return Theme::COL_OK;
    if (type == "damage") return Theme::COL_DANGER;
    return Theme::TEXT_PRIMARY;
}

const char* ScreenHUD::messagePrefix(const String& type) const {
    if (type == "kill")   return "[KILL]  ";
    if (type == "damage") return "[HIT]   ";
    return "[EVENT] ";
}

String ScreenHUD::relativeTime(unsigned long ts) const {
    if (ts == 0) return "";
    unsigned long elapsed = (millis() - ts) / 1000UL;
    if (elapsed < 60) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%lus", elapsed);
        return String(buf);
    }
    char buf[8];
    snprintf(buf, sizeof(buf), "%lum", elapsed / 60);
    return String(buf);
}
