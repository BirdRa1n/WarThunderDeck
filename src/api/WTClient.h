#pragma once
#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <freertos/semphr.h>
#include "WTModels.h"
#include "../config/Config.h"

class WTClient {
public:
    void begin(const String& hostIP);
    void pollAll(SharedData* data, SemaphoreHandle_t mutex);

    bool isServerReachable(const String& hostIP);

private:
    String     _base;
    WiFiClient _wifiClient;

    bool getJSON(const char* path, JsonDocument& doc);
    void parseIndicators(JsonDocument& doc, IndicatorData& out);
    void parseState(JsonDocument& doc, GameState& out);
    void parseMap(JsonDocument& doc, MapData& out);
    void parseHud(JsonDocument& doc, HudData& out);
};
