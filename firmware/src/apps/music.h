#pragma once
#include "app.h"

#include "font/roboto_thin_bold_24.h"
#include "font/roboto_thin_20.h"

const uint8_t AUTHOR_LENGTH = 24;
const uint8_t TRACK_LENGTH = 48;

class MusicApp : public App
{
public:
    MusicApp(TFT_eSprite *spr_, std::string entity_name);
    TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);
    uint8_t navigationNext();

private:
    std::string entity_name;
    uint8_t current_volume = 0;
    uint8_t last_volume = 0;
    uint8_t current_volume_position = 0;
    char author[AUTHOR_LENGTH];
    char track[TRACK_LENGTH];
};