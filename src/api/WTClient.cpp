#include "WTClient.h"
#include <Arduino.h>

void WTClient::begin(const String& hostIP) {
    _base = "http://" + hostIP + ":" + String(WT_PORT);
}

bool WTClient::isServerReachable(const String& hostIP) {
    begin(hostIP);
    JsonDocument doc;
    return getJSON("/state", doc) && doc["valid"].as<bool>();
}

bool WTClient::getJSON(const char* path, JsonDocument& doc) {
    HTTPClient http;
    http.begin(_wifiClient, _base + path);
    http.setTimeout(HTTP_TIMEOUT_MS);

    int code = http.GET();
    if (code != 200) {
        http.end();
        return false;
    }

    String body = http.getString();
    http.end();

    DeserializationError err = deserializeJson(doc, body);
    return err == DeserializationError::Ok;
}

void WTClient::parseIndicators(JsonDocument& doc, IndicatorData& out) {
    out.valid       = doc["valid"]            | false;
    out.vehicleType = doc["type"].as<String>();
    out.speed       = doc["speed"]            | 0.f;
    out.altitude    = doc["altitude_min"]     | 0.f;
    out.compass     = doc["compass"]          | 0.f;
    out.rpm         = doc["rpm_hour"]         | 0.f;
    out.oilPressure = doc["oil_pressure"]     | 0.f;
    out.headTemp    = doc["head_temperature"] | 0.f;
    out.fuel        = doc["fuel"]             | 0.f;
    out.throttle    = doc["throttle"]         | 0.f;
    out.flaps       = doc["flaps"]            | 0.f;
    out.gears       = doc["gears"]            | 0.f;
    out.gMeter      = doc["g_meter"]          | 0.f;
    out.gMeterMin   = doc["g_meter_min"]      | 0.f;
    out.gMeterMax   = doc["g_meter_max"]      | 0.f;
    out.mach        = doc["mach"]             | 0.f;
    out.lastUpdate  = millis();
}

void WTClient::parseState(JsonDocument& doc, GameState& out) {
    out.valid    = doc["valid"]    | false;
    out.health   = doc["health"]   | 0.f;
    out.speed    = doc["speed"]    | 0.f;
    out.altitude = doc["altitude"] | 0.f;
    out.heading  = doc["heading"]  | 0.f;
    out.throttle = doc["throttle"] | 0.f;
    out.fire     = doc["fire"]     | false;
    out.lastUpdate = millis();
}

void WTClient::parseMap(JsonDocument& doc, MapData& out) {
    out.valid = doc["valid"] | false;
    out.objects.clear();

    JsonArray objs = doc["objects"].as<JsonArray>();
    for (JsonObject o : objs) {
        MapObject mo;
        mo.type  = o["type"].as<String>();
        mo.color = o["color"].as<String>();
        mo.icon  = o["icon"].as<String>();
        mo.x     = o["x"] | 0.f;
        mo.y     = o["y"] | 0.f;
        out.objects.push_back(mo);
    }
    out.lastUpdate = millis();
}

void WTClient::parseHud(JsonDocument& doc, HudData& out) {
    out.valid = true;
    unsigned long now = millis();

    // Keep up to 6 most recent messages total
    std::vector<HudMessage> fresh;

    auto addMessages = [&](const char* category, const char* typeStr) {
        JsonArray arr = doc[category].as<JsonArray>();
        for (JsonObject m : arr) {
            HudMessage hm;
            hm.type      = typeStr;
            hm.text      = m["msg"].as<String>();
            if (hm.text.isEmpty()) hm.text = m["text"].as<String>();
            hm.timestamp = now;
            if (!hm.text.isEmpty()) fresh.push_back(hm);
        }
    };

    addMessages("kills",  "kill");
    addMessages("damage", "damage");
    addMessages("events", "event");

    // Prepend new messages, keep last 6
    for (auto it = fresh.rbegin(); it != fresh.rend(); ++it) {
        out.messages.insert(out.messages.begin(), *it);
    }
    if (out.messages.size() > 6) {
        out.messages.resize(6);
    }
    out.lastUpdate = now;
}

void WTClient::pollAll(SharedData* data, SemaphoreHandle_t mutex) {
    if (WiFi.status() != WL_CONNECTED) return;

    JsonDocument doc;
    bool anyValid = false;

    if (getJSON("/indicators", doc)) {
        IndicatorData tmp;
        parseIndicators(doc, tmp);
        if (xSemaphoreTake(mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            data->indicators = tmp;
            if (tmp.valid) anyValid = true;
            xSemaphoreGive(mutex);
        }
        doc.clear();
    }

    if (getJSON("/state", doc)) {
        GameState tmp;
        parseState(doc, tmp);
        if (xSemaphoreTake(mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            data->state = tmp;
            if (tmp.valid) anyValid = true;
            xSemaphoreGive(mutex);
        }
        doc.clear();
    }

    if (getJSON("/map_obj.json", doc)) {
        MapData tmp;
        parseMap(doc, tmp);
        // Also grab map name
        JsonDocument infoDoc;
        if (getJSON("/map_info.json", infoDoc)) {
            tmp.mapName = infoDoc["map_name"].as<String>();
        }
        if (xSemaphoreTake(mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            data->map = tmp;
            xSemaphoreGive(mutex);
        }
        doc.clear();
    }

    if (getJSON("/hudmsg", doc)) {
        HudData tmp;
        if (xSemaphoreTake(mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            tmp.messages = data->hud.messages; // carry over history
            xSemaphoreGive(mutex);
        }
        parseHud(doc, tmp);
        if (xSemaphoreTake(mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            data->hud = tmp;
            xSemaphoreGive(mutex);
        }
        doc.clear();
    }

    if (xSemaphoreTake(mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        data->wtConnected = anyValid;
        xSemaphoreGive(mutex);
    }
}
