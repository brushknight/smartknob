#include "app.h"

#include "font/roboto_thin_bold_24.h"
#include "font/roboto_thin_20.h"
#include "font/roboto_light_60.h"

class ClimateApp : public App
{
public:
    ClimateApp(TFT_eSprite *spr_);
    TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);
    uint8_t navigationNext();

private:
    uint8_t current_temperature = 0;
    uint8_t wanted_temperature = 0;
    uint8_t num_positions;

    // needed for UI
    float adjusted_sub_position = 0;
};