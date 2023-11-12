#include "app.h"

// #include "font/roboto_thin_bold_24.h"
#include "font/roboto_thin_20.h"
#include "font/roboto_light_60.h"

class BlindsApp : public App
{
public:
    BlindsApp(TFT_eSprite *spr_, std::string entity_name);
    TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);
    uint8_t navigationNext();

private:
    std::string entity_name;
    uint8_t current_closed_position = 0;
    uint8_t last_closed_position = 0;
    char buf_[24];
};