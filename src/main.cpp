#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <esp_task_wdt.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

#include "config/Config.h"
#include "config/UserConfig.h"
#include "api/WTClient.h"
#include "ui/TouchHandler.h"
#include "screens/ScreenManager.h"

// ─── Globals ─────────────────────────────────────────────────────────────────
static TFT_eSPI       tft;
static SPIClass       hspi(HSPI);   // barramento dedicado ao XPT2046
static TouchHandler   touch;
static UserConfig     userCfg;
static WTClient       wtClient;
static SharedData     sharedData;
static SemaphoreHandle_t dataMutex = nullptr;
static ScreenManager* mgr         = nullptr;

// ─── Polling task (Core 0) ───────────────────────────────────────────────────
struct PollTaskParam {
    WTClient*        client;
    SharedData*      data;
    SemaphoreHandle_t mutex;
    UserConfig*      cfg;
};

static void pollTask(void* param) {
    auto* p = static_cast<PollTaskParam*>(param);

    esp_task_wdt_add(nullptr);

    while (true) {
        esp_task_wdt_reset();

        if (WiFi.status() == WL_CONNECTED) {
            p->client->begin(p->cfg->pcIP);
            p->client->pollAll(p->data, p->mutex);
        } else {
            // Attempt reconnect
            WiFi.reconnect();
            if (xSemaphoreTake(p->mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
                p->data->wtConnected = false;
                xSemaphoreGive(p->mutex);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(POLL_INTERVAL_MS));
    }
}

// ─── Backlight PWM ───────────────────────────────────────────────────────────
static void setBrightness(uint8_t level) {
    ledcWrite(BL_PWM_CHANNEL, level);
}

// ─── setup() ─────────────────────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    Serial.println("[WarThunderDeck] booting...");

    // RGB LED off (active-low on CYD)
    pinMode(PIN_LED_RED,   OUTPUT); digitalWrite(PIN_LED_RED,   HIGH);
    pinMode(PIN_LED_GREEN, OUTPUT); digitalWrite(PIN_LED_GREEN, HIGH);
    pinMode(PIN_LED_BLUE,  OUTPUT); digitalWrite(PIN_LED_BLUE,  HIGH);

    // Backlight PWM (Arduino ESP32 v2.x API)
    ledcSetup(BL_PWM_CHANNEL, BL_PWM_FREQ, BL_PWM_BITS);
    ledcAttachPin(PIN_TFT_BL, BL_PWM_CHANNEL);
    setBrightness(BL_DEFAULT);

    // Display
    tft.init();
    tft.setRotation(0);  // portrait
    tft.fillScreen(TFT_BLACK);

    // NVS config
    userCfg.load();
    setBrightness(userCfg.brightness);

    // HSPI dedicado ao touch (XPT2046 usa barramento separado do display)
    hspi.begin(PIN_TOUCH_CLK, PIN_TOUCH_DOUT, PIN_TOUCH_DIN, PIN_TOUCH_CS);
    touch.begin(hspi);

    // Mutex
    dataMutex = xSemaphoreCreateMutex();
    configASSERT(dataMutex);

    // Watchdog
    esp_task_wdt_init(WDT_TIMEOUT_S, true);
    esp_task_wdt_add(nullptr);

    // Screen manager (may launch WiFiManager if not yet configured)
    mgr = new ScreenManager(userCfg);
    mgr->begin(&tft);

    // Polling task on Core 0
    static PollTaskParam pollParam = { &wtClient, &sharedData, dataMutex, &userCfg };
    xTaskCreatePinnedToCore(
        pollTask,
        "wt_poll",
        POLL_TASK_STACK,
        &pollParam,
        POLL_TASK_PRIO,
        nullptr,
        POLL_TASK_CORE
    );

    // Blue LED = ready
    digitalWrite(PIN_LED_BLUE, LOW);
    Serial.println("[WarThunderDeck] ready");
}

// ─── loop() — UI task on Core 1 ──────────────────────────────────────────────
void loop() {
    esp_task_wdt_reset();

    // Touch input
    uint16_t tx, ty;
    if (touch.getTouch(tx, ty)) {
        mgr->handleTouch(tx, ty);
    }

    // Update model from shared data, then redraw
    mgr->update(sharedData, dataMutex);
    mgr->draw();

    vTaskDelay(pdMS_TO_TICKS(16)); // ~60 fps ceiling
}
