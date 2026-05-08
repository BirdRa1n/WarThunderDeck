#pragma once
#include <TFT_eSPI.h>
#include <cmath>

namespace Theme {

// ─── Core palette (RGB565) ───────────────────────────────────────────────────
constexpr uint16_t BG           = TFT_BLACK;
constexpr uint16_t SURFACE      = 0x1082;   // dark grey
constexpr uint16_t SURFACE2     = 0x2104;   // slightly lighter
constexpr uint16_t BORDER       = 0x4208;   // mid grey
constexpr uint16_t TEXT_PRIMARY = TFT_WHITE;
constexpr uint16_t TEXT_DIM     = 0xBDF7;   // light grey
constexpr uint16_t TEXT_LABEL   = 0x7BEF;   // medium grey

// ─── Status colours ──────────────────────────────────────────────────────────
constexpr uint16_t COL_OK     = 0x07E0;   // green
constexpr uint16_t COL_WARN   = 0xFFE0;   // yellow
constexpr uint16_t COL_DANGER = 0xF800;   // red
constexpr uint16_t COL_INFO   = 0x001F;   // blue
constexpr uint16_t COL_CYAN   = 0x07FF;
constexpr uint16_t COL_ORANGE = 0xFBE0;

// ─── Tab bar ─────────────────────────────────────────────────────────────────
constexpr uint16_t TAB_BG          = 0x2104;
constexpr uint16_t TAB_ACTIVE_BG   = 0x0458;   // dark teal
constexpr uint16_t TAB_TEXT        = 0xBDF7;
constexpr uint16_t TAB_ACTIVE_TEXT = 0x07FF;   // cyan

// ─── Map object colours ──────────────────────────────────────────────────────
constexpr uint16_t MAP_BG     = 0x0841;   // very dark green-grey
constexpr uint16_t MAP_GRID   = 0x2104;
constexpr uint16_t MAP_PLAYER = COL_OK;
constexpr uint16_t MAP_ALLY   = COL_CYAN;
constexpr uint16_t MAP_ENEMY  = COL_DANGER;
constexpr uint16_t MAP_OTHER  = 0x8410;   // grey

// ─── Thresholds ──────────────────────────────────────────────────────────────
constexpr float HEALTH_WARN   = 50.f;
constexpr float HEALTH_CRIT   = 25.f;
constexpr float G_WARN        =  5.f;
constexpr float G_CRIT        =  7.f;
constexpr float FUEL_WARN     = 500.f;   // raw units
constexpr float RPM_WARN_PCT  = 0.85f;   // 85% of gMeterMax used as proxy

// ─── Colour helpers ──────────────────────────────────────────────────────────
inline uint16_t healthColor(float h) {
    if (h <= HEALTH_CRIT) return COL_DANGER;
    if (h <= HEALTH_WARN) return COL_WARN;
    return COL_OK;
}

inline uint16_t gColor(float g) {
    float ag = fabsf(g);
    if (ag >= G_CRIT) return COL_DANGER;
    if (ag >= G_WARN) return COL_WARN;
    return COL_OK;
}

inline uint16_t fuelColor(float f) {
    if (f < FUEL_WARN * 0.5f) return COL_DANGER;
    if (f < FUEL_WARN)        return COL_WARN;
    return COL_OK;
}

inline uint16_t mapObjectColor(const char* colorStr) {
    if (!colorStr) return MAP_OTHER;
    if (strcmp(colorStr, "green") == 0) return MAP_PLAYER;
    if (strcmp(colorStr, "blue")  == 0) return MAP_ALLY;
    if (strcmp(colorStr, "red")   == 0) return MAP_ENEMY;
    return MAP_OTHER;
}

} // namespace Theme
