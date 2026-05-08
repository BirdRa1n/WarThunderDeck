#pragma once
#include "BaseScreen.h"
#include "../config/UserConfig.h"

class ScreenSetup : public BaseScreen {
public:
    explicit ScreenSetup(UserConfig& cfg) : _cfg(cfg) {}

    void begin(TFT_eSPI* tft) override;
    void onActivate() override;
    void update(const SharedData& data) override {}
    void draw() override;
    void onTouch(uint16_t x, uint16_t y) override;

    // Returns true once setup is complete and the main screens can be shown.
    bool isReady() const { return _ready; }

private:
    enum class State { SHOW_INSTRUCTIONS, WAITING_WIFI, WIFI_DONE, IP_ENTRY, TESTING, ERROR, READY };

    UserConfig& _cfg;
    State       _state    = State::SHOW_INSTRUCTIONS;
    bool        _ready    = false;
    bool        _dirty    = true;
    String      _ipBuffer = "192.168.1.100";
    String      _errorMsg;

    // ── Numeric keypad ─────────────────────────────────────────────────────
    static constexpr int KEY_COLS  = 3;
    static constexpr int KEY_ROWS  = 4;
    static constexpr int KEY_X0    = 0;
    static constexpr int KEY_Y0    = 42;   // leaves room for IP display bar at top
    static constexpr int KEY_W     = 80;
    static constexpr int KEY_H     = 44;   // 4×44 = 176; ends at y=218
    // Connect button: y=220, h=34  (y=220-254)
    // Re-setup button: y=256, h=34 (y=256-290)
    static constexpr int BTN_CONN_Y  = 220;
    static constexpr int BTN_WIFI_Y  = 256;
    static constexpr int BTN_H       = 34;

    const char* _keyLabels[KEY_ROWS][KEY_COLS] = {
        {"1","2","3"},
        {"4","5","6"},
        {"7","8","9"},
        {".","0","<"}
    };

    void drawInstructions();
    void drawIPEntry();
    void drawStatus(const char* msg, uint16_t color);
    void startWiFiManager();
    void testConnection();
    bool processKey(uint16_t x, uint16_t y);
};
