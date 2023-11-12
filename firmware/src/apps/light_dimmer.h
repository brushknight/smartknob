#include "app.h"

#include "font/roboto_thin_bold_24.h"
#include "font/roboto_thin_20.h"

class LightDimmerApp : public App
{
public:
    LightDimmerApp(TFT_eSprite *spr_, std::string entity_name);
    TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);
    uint8_t navigationNext();

private:
    uint16_t current_position = 0;
    uint16_t last_position = 0;
    uint8_t num_positions = 0;
    std::string entity_name;
    // needed for UI
    float sub_position_unit = 0;
    float adjusted_sub_position = 0;
};