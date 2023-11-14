#pragma once

#include <Arduino.h>
#include "configuration.h"

// TODO: move it into the app.h
const uint32_t APP_ID_SETTINGS = 7;
const uint32_t APP_ID_HOME_ASSISTANT = 6;

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
    PB_SmartKnobState motor_state;
    ConnectivityState connectivity_state;
};

struct EntityStateUpdate
{
    std::string entity_name;
    float new_value = 0;
    char app_slug[48] = "";
    bool changed = false;
    bool sent = false;
};