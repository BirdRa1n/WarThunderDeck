#include "UserConfig.h"

void UserConfig::load() {
    _prefs.begin(NVS_NAMESPACE, true);
    pcIP          = _prefs.getString("pcip",   pcIP);
    defaultScreen = _prefs.getUChar("defscr",  defaultScreen);
    brightness    = _prefs.getUChar("bright",  brightness);
    _prefs.end();
}

void UserConfig::save() {
    _prefs.begin(NVS_NAMESPACE, false);
    _prefs.putString("pcip",  pcIP);
    _prefs.putUChar("defscr", defaultScreen);
    _prefs.putUChar("bright", brightness);
    _prefs.end();
}

void UserConfig::reset() {
    _prefs.begin(NVS_NAMESPACE, false);
    _prefs.clear();
    _prefs.end();
    pcIP          = "192.168.1.100";
    defaultScreen = SCREEN_INDICATORS;
    brightness    = BL_DEFAULT;
}
