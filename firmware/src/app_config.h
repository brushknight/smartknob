#pragma once

#include <Arduino.h>
#include "configuration.h"

struct AppConfig
{
    std::string name;
    uint32_t ui_id; // for linking with display renderer
    int32_t current_position;
    int32_t last_position;
    int32_t positions_count; // max positions count

    PB_SmartKnobConfig motor_config;
};

struct AppState
{
    std::string name;
    uint32_t ui_id; // for linking with display renderer
    int32_t current_position;

    PB_SmartKnobState motor_state;
};