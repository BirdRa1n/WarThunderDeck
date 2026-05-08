#include "ScreenIndicators.h"
#include "../ui/Widgets.h"
#include "../ui/Theme.h"
#include "../config/Config.h"

// ─── Layout constants ────────────────────────────────────────────────────────
static constexpr int16_t Y_VEHICLE   =  4;
static constexpr int16_t Y_SPD_LABEL = 24;
static constexpr int16_t Y_SPD_VAL   = 38;
static constexpr int16_t Y_THR_LABEL = 90;
static constexpr int16_t Y_THR_BAR   = 104;
static constexpr int16_t Y_FUEL_LABEL = 122;
static constexpr int16_t Y_FUEL_BAR  = 136;
static constexpr int16_t Y_RPM_LABEL = 156;
static constexpr int16_t Y_RPM_VAL   = 170;
static constexpr int16_t Y_G_LABEL   = 198;
static constexpr int16_t Y_G_VAL     = 212;
static constexpr int16_t Y_GEAR_ROW  = 250;
static constexpr int16_t BAR_X       =   4;
static constexpr int16_t BAR_W       = SCREEN_W - 8;
static constexpr int16_t BAR_H       =  14;

void ScreenIndicators::begin(TFT_eSPI* tft) {
    _tft = tft;
}

void ScreenIndicators::onActivate() {
    _fullRedraw = true;
}

void ScreenIndicators::update(const SharedData& d) {
    _prev      = _data;
    _data      = d.indicators;
    _connected = d.wtConnected;
}

void ScreenIndicators::draw() {
    if (_fullRedraw) {
        _tft->fillRect(0, 0, SCREEN_W, CONTENT_H, Theme::BG);
        drawStatic();
        _fullRedraw = false;
    }
    drawDynamic();
}

// ─── Static chrome (labels, dividers) ────────────────────────────────────────

void ScreenIndicators::drawStatic() {
    Widgets::drawDivider(*_tft, 0, Y_THR_LABEL - 4, SCREEN_W);
    Widgets::drawDivider(*_tft, 0, Y_RPM_LABEL - 4, SCREEN_W);
    Widgets::drawDivider(*_tft, 0, Y_GEAR_ROW  - 4, SCREEN_W);
    Widgets::drawDivider(*_tft, SCREEN_W / 2, Y_SPD_LABEL, 1);

    Widgets::drawLabel(*_tft, 4,            Y_SPD_LABEL,  "SPD km/h");
    Widgets::drawLabel(*_tft, SCREEN_W / 2 + 4, Y_SPD_LABEL, "ALT m");

    Widgets::drawLabel(*_tft, 4, Y_THR_LABEL, "THROTTLE");
    Widgets::drawLabel(*_tft, 4, Y_FUEL_LABEL, "FUEL");

    Widgets::drawLabel(*_tft, 4,            Y_RPM_LABEL, "RPM");
    Widgets::drawLabel(*_tft, SCREEN_W / 2 + 4, Y_RPM_LABEL, "MACH");

    Widgets::drawLabel(*_tft, 4,            Y_G_LABEL, "G-FORCE");
    Widgets::drawLabel(*_tft, SCREEN_W / 2 + 4, Y_G_LABEL, "HDG");

    Widgets::drawLabel(*_tft, 4,            Y_GEAR_ROW + 8, "GEAR");
    Widgets::drawLabel(*_tft, SCREEN_W / 2 + 4, Y_GEAR_ROW + 8, "FLAPS");
}

// ─── Dynamic values ──────────────────────────────────────────────────────────

void ScreenIndicators::drawDynamic() {
    if (!_connected || !_data.valid) {
        Widgets::drawWaitOverlay(*_tft);
        return;
    }

    drawVehicleName();
    drawSpeedAlt();
    drawThrottleBar();
    drawFuelBar();
    drawRpmMach();
    drawGForce();
    drawGearFlaps();
}

void ScreenIndicators::drawVehicleName() {
    _tft->fillRect(0, Y_VEHICLE, SCREEN_W, 18, Theme::BG);
    _tft->setTextColor(Theme::TEXT_DIM, Theme::BG);
    _tft->setTextFont(2);
    _tft->setTextDatum(MC_DATUM);
    _tft->drawString(_data.vehicleType.c_str(), SCREEN_W / 2, Y_VEHICLE + 8);
    _tft->setTextDatum(TL_DATUM);
}

void ScreenIndicators::drawSpeedAlt() {
    char buf[16];

    // Speed
    snprintf(buf, sizeof(buf), "%4.0f", _data.speed);
    _tft->fillRect(0, Y_SPD_VAL, SCREEN_W / 2 - 1, 46, Theme::BG);
    Widgets::drawBigValue(*_tft, 4, Y_SPD_VAL, buf, Theme::TEXT_PRIMARY, 6);

    // Altitude
    snprintf(buf, sizeof(buf), "%5.0f", _data.altitude);
    _tft->fillRect(SCREEN_W / 2 + 1, Y_SPD_VAL, SCREEN_W / 2 - 1, 46, Theme::BG);
    Widgets::drawBigValue(*_tft, SCREEN_W / 2 + 4, Y_SPD_VAL, buf, Theme::TEXT_PRIMARY, 6);
}

void ScreenIndicators::drawThrottleBar() {
    char buf[10];
    snprintf(buf, sizeof(buf), "%3.0f%%", _data.throttle * 100.f);
    _tft->fillRect(BAR_X + BAR_W - 40, Y_THR_LABEL, 40, 14, Theme::BG);
    Widgets::drawLabel(*_tft, BAR_X + BAR_W - 40, Y_THR_LABEL, buf, Theme::TEXT_PRIMARY);
    Widgets::drawBar(*_tft, BAR_X, Y_THR_BAR, BAR_W, BAR_H,
                    _data.throttle, 1.f, Theme::COL_OK);
}

void ScreenIndicators::drawFuelBar() {
    uint16_t col = Theme::fuelColor(_data.fuel);

    // Blink the label when fuel is critical
    bool blinkOn = (_blinkCount++ % 6 < 3);
    if (_data.fuel < Theme::FUEL_WARN * 0.5f && !blinkOn) {
        col = Theme::BG;
    }

    char buf[12];
    snprintf(buf, sizeof(buf), "%.0f u", _data.fuel);
    _tft->fillRect(BAR_X + BAR_W - 60, Y_FUEL_LABEL, 60, 14, Theme::BG);
    Widgets::drawLabel(*_tft, BAR_X + BAR_W - 60, Y_FUEL_LABEL, buf, col);

    // Use a reference of ~4000 units for bar fill (typical aircraft)
    Widgets::drawBar(*_tft, BAR_X, Y_FUEL_BAR, BAR_W, BAR_H,
                    _data.fuel, 4000.f, col);
}

void ScreenIndicators::drawRpmMach() {
    char buf[12];

    // RPM
    uint16_t rpmCol = (_data.rpm > 3000.f) ? Theme::COL_DANGER : Theme::TEXT_PRIMARY;
    snprintf(buf, sizeof(buf), "%.0f", _data.rpm);
    _tft->fillRect(0, Y_RPM_VAL, SCREEN_W / 2 - 1, 24, Theme::BG);
    Widgets::drawBigValue(*_tft, 4, Y_RPM_VAL, buf, rpmCol, 4);

    // Mach
    snprintf(buf, sizeof(buf), "%.2f", _data.mach);
    _tft->fillRect(SCREEN_W / 2 + 1, Y_RPM_VAL, SCREEN_W / 2 - 1, 24, Theme::BG);
    Widgets::drawBigValue(*_tft, SCREEN_W / 2 + 4, Y_RPM_VAL, buf, Theme::TEXT_PRIMARY, 4);
}

void ScreenIndicators::drawGForce() {
    char buf[12];

    // G-force
    uint16_t gCol = Theme::gColor(_data.gMeter);
    snprintf(buf, sizeof(buf), "%+.1fG", _data.gMeter);
    _tft->fillRect(0, Y_G_VAL, SCREEN_W / 2 - 1, 24, Theme::BG);
    Widgets::drawBigValue(*_tft, 4, Y_G_VAL, buf, gCol, 4);

    // Heading
    snprintf(buf, sizeof(buf), "%3.0f\xb0", _data.compass);
    _tft->fillRect(SCREEN_W / 2 + 1, Y_G_VAL, SCREEN_W / 2 - 1, 24, Theme::BG);
    Widgets::drawBigValue(*_tft, SCREEN_W / 2 + 4, Y_G_VAL, buf, Theme::COL_WARN, 4);
}

void ScreenIndicators::drawGearFlaps() {
    _tft->fillRect(0, Y_GEAR_ROW, SCREEN_W, CONTENT_H - Y_GEAR_ROW, Theme::BG);

    bool gearDown  = (_data.gears  > 0.5f);
    bool flapsDown = (_data.flaps  > 0.1f);

    Widgets::drawLabel(*_tft, 40, Y_GEAR_ROW + 8, gearDown  ? "DOWN" : "UP",
                       gearDown  ? Theme::COL_WARN : Theme::COL_OK);
    Widgets::drawStateIcon(*_tft, 26, Y_GEAR_ROW + 8, gearDown,  Theme::COL_WARN);

    Widgets::drawLabel(*_tft, SCREEN_W / 2 + 40, Y_GEAR_ROW + 8, flapsDown ? "DOWN" : "UP",
                       flapsDown ? Theme::COL_WARN : Theme::COL_OK);
    Widgets::drawStateIcon(*_tft, SCREEN_W / 2 + 26, Y_GEAR_ROW + 8, flapsDown, Theme::COL_WARN);
}
