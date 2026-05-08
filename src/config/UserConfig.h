#pragma once
#include <Arduino.h>
#include <Preferences.h>
#include "Config.h"

class UserConfig {
public:
    String  pcIP          = "192.168.1.100";
    uint8_t defaultScreen = SCREEN_INDICATORS;
    uint8_t brightness    = BL_DEFAULT;

    void load();
    void save();
    void reset();

private:
    Preferences _prefs;
};
