#pragma once

// ─── Display pins ────────────────────────────────────────────────────────────
#define PIN_TFT_MISO    12
#define PIN_TFT_MOSI    13
#define PIN_TFT_SCLK    14
#define PIN_TFT_CS      15
#define PIN_TFT_DC       2
#define PIN_TFT_RST     -1
#define PIN_TFT_BL      21

// ─── Touch pins ──────────────────────────────────────────────────────────────
#define PIN_TOUCH_CS    33
#define PIN_TOUCH_IRQ   36

// ─── LED RGB onboard ─────────────────────────────────────────────────────────
#define PIN_LED_RED      4
#define PIN_LED_GREEN   16
#define PIN_LED_BLUE    17

// ─── LDR (light sensor) ──────────────────────────────────────────────────────
#define PIN_LDR         34

// ─── Backlight PWM ───────────────────────────────────────────────────────────
#define BL_PWM_CHANNEL   0
#define BL_PWM_FREQ   5000
#define BL_PWM_BITS      8
#define BL_DEFAULT     200   // 0–255

// ─── Display dimensions ──────────────────────────────────────────────────────
#define SCREEN_W         240
#define SCREEN_H         320
#define TAB_BAR_H         20
#define CONTENT_H        (SCREEN_H - TAB_BAR_H)   // 300

// ─── Touch calibration (CYD typical values, portrait) ────────────────────────
#define TS_MINX  349
#define TS_MINY  299
#define TS_MAXX 3859
#define TS_MAXY 3917

// ─── Network ─────────────────────────────────────────────────────────────────
#define WT_PORT           8111
#define HTTP_TIMEOUT_MS   2000
#define POLL_INTERVAL_MS   500

// ─── NVS namespace ───────────────────────────────────────────────────────────
#define NVS_NAMESPACE  "wtdeck"

// ─── Watchdog ────────────────────────────────────────────────────────────────
#define WDT_TIMEOUT_S     30

// ─── FreeRTOS polling task ────────────────────────────────────────────────────
#define POLL_TASK_STACK  8192
#define POLL_TASK_PRIO      1
#define POLL_TASK_CORE      0

// ─── WiFiManager AP ──────────────────────────────────────────────────────────
#define WIFI_AP_NAME  "WarThunderDeck"
#define WIFI_AP_PASS  "warthunder"

// ─── Screen indices ──────────────────────────────────────────────────────────
#define SCREEN_INDICATORS  0
#define SCREEN_MAP         1
#define SCREEN_STATE       2
#define SCREEN_HUD         3
#define SCREEN_COUNT       4
