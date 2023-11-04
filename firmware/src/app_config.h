#pragma once

#include <Arduino.h>
#include "configuration.h"

// TODO: move it into the app.h
const uint32_t APP_ID_SETTINGS = 7;
const uint32_t APP_ID_HOME_ASSISTANT = 6;

struct AppConfig
{
    std::string name;
    uint32_t ui_id; // for linking with display renderer
    int32_t current_position;
    int32_t last_position;
    int32_t positions_count; // max positions count

    PB_SmartKnobConfig motor_config;
};

struct ConnectivityState
{
    bool is_connected;
    int8_t signal_strength;
    /* *
0 - Excellent
1 - Good
2 - Fair
3 - Poor
4 - No signal
    */
    uint8_t signal_strenth_status;
    std::string ssid;
    std::string ip_address;
};

struct AppState
{
    std::string name;
    uint32_t ui_id; // for linking with display renderer
    int32_t current_position;
    int32_t positions_count; // max positions count

    PB_SmartKnobState motor_state;
    ConnectivityState connectivity_state;
};
