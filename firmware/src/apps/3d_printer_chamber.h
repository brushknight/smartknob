#include "app.h"

#include "font/roboto_thin_bold_24.h"
#include "font/roboto_thin_20.h"
#include "font/roboto_light_60.h"

class PrinterChamberApp : public App
{
public:
    PrinterChamberApp(TFT_eSprite *spr_);
    TFT_eSprite *render();
    void updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);
    uint8_t navigationNext();

private:
    uint8_t current_temperature = 0;
    uint8_t wanted_temperature = 0;
    uint32_t wanted_temperature_position = 0;
    uint8_t num_positions;
    uint8_t degrees_per_position;
    char buf_[64];
};