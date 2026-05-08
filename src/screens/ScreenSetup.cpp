#include "ScreenSetup.h"
#include <WiFiManager.h>
#include <HTTPClient.h>
#include "../config/Config.h"
#include "../ui/Theme.h"
#include "../ui/Widgets.h"

void ScreenSetup::begin(TFT_eSPI* tft) {
    _tft = tft;
}

void ScreenSetup::onActivate() {
    _dirty = true;
    _state = State::SHOW_INSTRUCTIONS;
    _ready = false;
}

void ScreenSetup::draw() {
    if (!_dirty) return;
    _dirty = false;

    _tft->fillScreen(Theme::BG);

    switch (_state) {
        case State::SHOW_INSTRUCTIONS:
            drawInstructions();
            break;
        case State::WAITING_WIFI:
            drawStatus("Starting WiFi portal...", Theme::COL_WARN);
            break;
        case State::WIFI_DONE:
            drawIPEntry();
            break;
        case State::IP_ENTRY:
            drawIPEntry();
            break;
        case State::TESTING:
            drawStatus("Testing connection...", Theme::COL_WARN);
            break;
        case State::ERROR:
            drawIPEntry();
            if (!_errorMsg.isEmpty()) {
                _tft->setTextColor(Theme::COL_DANGER, Theme::BG);
                _tft->setTextFont(2);
                _tft->setTextDatum(MC_DATUM);
                _tft->drawString(_errorMsg.c_str(), SCREEN_W / 2, 68);
                _tft->setTextDatum(TL_DATUM);
            }
            break;
        case State::READY:
            drawStatus("Connected! Starting...", Theme::COL_OK);
            break;
    }
}

void ScreenSetup::onTouch(uint16_t x, uint16_t y) {
    switch (_state) {
        case State::SHOW_INSTRUCTIONS: {
            // "Setup WiFi" button at bottom
            if (y > CONTENT_H - 50 && y < CONTENT_H) {
                _state = State::WAITING_WIFI;
                _dirty = true;
                draw();  // force immediate draw before blocking call
                startWiFiManager();
            }
            break;
        }
        case State::WIFI_DONE:
        case State::IP_ENTRY:
        case State::ERROR: {
            if (processKey(x, y)) {
                _dirty = true;
            }
            // "Connect" button
            if (y >= BTN_CONN_Y && y < BTN_CONN_Y + BTN_H) {
                _state = State::TESTING;
                _dirty = true;
                draw();
                testConnection();
            }
            // "Re-setup WiFi" button
            if (y >= BTN_WIFI_Y && y < BTN_WIFI_Y + BTN_H) {
                _state = State::WAITING_WIFI;
                _dirty = true;
                draw();
                startWiFiManager();
            }
            break;
        }
        default: break;
    }
}

// ─── Private helpers ─────────────────────────────────────────────────────────

void ScreenSetup::drawInstructions() {
    _tft->setTextColor(Theme::COL_CYAN, Theme::BG);
    _tft->setTextFont(4);
    _tft->setTextDatum(MC_DATUM);
    _tft->drawString("WarThunderDeck", SCREEN_W / 2, 30);

    _tft->setTextColor(Theme::TEXT_PRIMARY, Theme::BG);
    _tft->setTextFont(2);
    _tft->drawString("First-time setup", SCREEN_W / 2, 60);

    _tft->setTextColor(Theme::TEXT_DIM, Theme::BG);
    _tft->setTextFont(2);
    _tft->drawString("Connect to WiFi:", SCREEN_W / 2, 100);
    _tft->setTextColor(Theme::COL_OK, Theme::BG);
    _tft->drawString(WIFI_AP_NAME, SCREEN_W / 2, 118);
    _tft->setTextColor(Theme::TEXT_DIM, Theme::BG);
    _tft->drawString("Password: " WIFI_AP_PASS, SCREEN_W / 2, 136);
    _tft->drawString("Visit 192.168.4.1", SCREEN_W / 2, 154);
    _tft->drawString("to configure WiFi + PC IP", SCREEN_W / 2, 172);

    // "Start Setup" button
    _tft->fillRoundRect(20, CONTENT_H - 50, SCREEN_W - 40, 40, 6, Theme::TAB_ACTIVE_BG);
    _tft->setTextColor(Theme::COL_CYAN, Theme::TAB_ACTIVE_BG);
    _tft->setTextFont(4);
    _tft->drawString("Start WiFi Setup", SCREEN_W / 2, CONTENT_H - 36);
    _tft->setTextDatum(TL_DATUM);
}

void ScreenSetup::drawIPEntry() {
    // IP display bar
    _tft->fillRect(0, 10, SCREEN_W, 28, Theme::SURFACE);
    _tft->setTextColor(Theme::TEXT_PRIMARY, Theme::SURFACE);
    _tft->setTextFont(4);
    _tft->setTextDatum(ML_DATUM);
    _tft->drawString(_ipBuffer.c_str(), 8, 24);
    _tft->setTextDatum(TL_DATUM);

    // Keypad
    for (int r = 0; r < KEY_ROWS; r++) {
        for (int c = 0; c < KEY_COLS; c++) {
            int16_t kx = KEY_X0 + c * KEY_W;
            int16_t ky = KEY_Y0 + r * KEY_H;
            _tft->fillRect(kx + 1, ky + 1, KEY_W - 2, KEY_H - 2, Theme::SURFACE);
            _tft->drawRect(kx, ky, KEY_W, KEY_H, Theme::BORDER);
            _tft->setTextColor(Theme::TEXT_PRIMARY, Theme::SURFACE);
            _tft->setTextFont(4);
            _tft->setTextDatum(MC_DATUM);
            _tft->drawString(_keyLabels[r][c], kx + KEY_W / 2, ky + KEY_H / 2);
        }
    }
    _tft->setTextDatum(TL_DATUM);

    // Connect button
    _tft->fillRoundRect(4, BTN_CONN_Y, SCREEN_W - 8, BTN_H, 4, Theme::COL_OK);
    _tft->setTextColor(Theme::BG, Theme::COL_OK);
    _tft->setTextFont(2);
    _tft->setTextDatum(MC_DATUM);
    _tft->drawString("CONNECT", SCREEN_W / 2, BTN_CONN_Y + BTN_H / 2);

    // Re-setup WiFi button
    _tft->fillRoundRect(4, BTN_WIFI_Y, SCREEN_W - 8, BTN_H, 4, Theme::SURFACE2);
    _tft->setTextColor(Theme::TEXT_DIM, Theme::SURFACE2);
    _tft->drawString("RE-SETUP WIFI", SCREEN_W / 2, BTN_WIFI_Y + BTN_H / 2);
    _tft->setTextDatum(TL_DATUM);
}

void ScreenSetup::drawStatus(const char* msg, uint16_t color) {
    _tft->setTextColor(color, Theme::BG);
    _tft->setTextFont(2);
    _tft->setTextDatum(MC_DATUM);
    _tft->drawString(msg, SCREEN_W / 2, CONTENT_H / 2);
    _tft->setTextDatum(TL_DATUM);
}

void ScreenSetup::startWiFiManager() {
    WiFiManager wm;
    wm.setConfigPortalTimeout(120);

    // Custom parameter for PC IP
    WiFiManagerParameter ipParam("pcip", "War Thunder PC IP", _cfg.pcIP.c_str(), 16);
    wm.addParameter(&ipParam);

    bool connected = wm.startConfigPortal(WIFI_AP_NAME, WIFI_AP_PASS);

    if (connected) {
        String newIP = String(ipParam.getValue());
        if (!newIP.isEmpty()) {
            _ipBuffer = newIP;
            _cfg.pcIP = newIP;
        }
        _state = State::WIFI_DONE;
    } else {
        _state = State::SHOW_INSTRUCTIONS;
    }
    _dirty = true;
}

void ScreenSetup::testConnection() {
    WiFiClient client;
    HTTPClient http;
    String url = "http://" + _ipBuffer + ":" + String(WT_PORT) + "/state";
    http.begin(client, url);
    http.setTimeout(HTTP_TIMEOUT_MS);
    int code = http.GET();
    http.end();

    if (code == 200) {
        _cfg.pcIP = _ipBuffer;
        _cfg.save();
        _state = State::READY;
        _dirty = true;
        draw();
        delay(1200);
        _ready = true;
    } else {
        char buf[40];
        snprintf(buf, sizeof(buf), "Failed (HTTP %d)", code);
        _errorMsg = buf;
        _state = State::ERROR;
        _dirty = true;
    }
}

bool ScreenSetup::processKey(uint16_t x, uint16_t y) {
    if (y < KEY_Y0) return false;
    int r = (y - KEY_Y0) / KEY_H;
    int c = (x - KEY_X0) / KEY_W;
    if (r >= KEY_ROWS || c >= KEY_COLS || r < 0 || c < 0) return false;

    const char* k = _keyLabels[r][c];
    if (strcmp(k, "<") == 0) {
        if (!_ipBuffer.isEmpty()) _ipBuffer.remove(_ipBuffer.length() - 1);
    } else {
        if (_ipBuffer.length() < 15) _ipBuffer += k;
    }
    _state = State::IP_ENTRY;
    return true;
}
