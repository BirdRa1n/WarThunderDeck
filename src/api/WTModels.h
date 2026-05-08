#pragma once
#include <Arduino.h>
#include <vector>

struct IndicatorData {
    bool   valid        = false;
    String vehicleType;
    float  speed        = 0.f;   // km/h
    float  altitude     = 0.f;   // metres
    float  compass      = 0.f;   // degrees 0–360
    float  rpm          = 0.f;
    float  oilPressure  = 0.f;
    float  headTemp     = 0.f;
    float  fuel         = 0.f;   // raw units from API
    float  throttle     = 0.f;   // 0.0–1.0
    float  flaps        = 0.f;   // 0.0–1.0
    float  gears        = 0.f;   // 0.0–1.0
    float  gMeter       = 0.f;
    float  gMeterMin    = 0.f;
    float  gMeterMax    = 0.f;
    float  mach         = 0.f;
    unsigned long lastUpdate = 0;
};

struct MapObject {
    String type;
    String color;   // "green","red","blue","grey"
    String icon;    // "Player", etc.
    float  x = 0.f; // 0.0–1.0 normalised
    float  y = 0.f;
};

struct MapData {
    bool   valid = false;
    String mapName;
    std::vector<MapObject> objects;
    unsigned long lastUpdate = 0;
};

struct GameState {
    bool  valid    = false;
    float health   = 0.f;   // 0–100
    float speed    = 0.f;
    float altitude = 0.f;
    float heading  = 0.f;
    float throttle = 0.f;
    bool  fire     = false;
    unsigned long lastUpdate = 0;
};

struct HudMessage {
    String type;       // "kill","damage","event"
    String text;
    unsigned long timestamp = 0;
};

struct HudData {
    bool valid = false;
    std::vector<HudMessage> messages;
    unsigned long lastUpdate = 0;
};

struct SharedData {
    IndicatorData indicators;
    MapData       map;
    GameState     state;
    HudData       hud;
    bool          wtConnected = false;
};
