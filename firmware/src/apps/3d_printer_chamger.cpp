#include "3d_printer_chamber.h"

PrinterChamberApp::PrinterChamberApp(TFT_eSprite *spr_, std::string hass_entity_name) : App(spr_)
{
    current_temperature = 30;
    this->entity_name = entity_name;
    uint8_t min_temp = 0;
    uint8_t max_temp = 120;
    float segment_size = 230.0 / 21.0 * PI / 180;
    degrees_per_position = 5;

    motor_config = PB_SmartKnobConfig{
        current_temperature / degrees_per_position,
        0,
        current_temperature / degrees_per_position,
        min_temp / degrees_per_position,
        max_temp / degrees_per_position,
        segment_size,
        2,
        1,
        1.1,
        "", // TODO: clean this
        0,
        {},
        0,
        90,
    };

    num_positions = motor_config.max_position - motor_config.min_position;
}

uint8_t PrinterChamberApp::navigationNext()
{
    // back to menu
    return 0;
}

EntityStateUpdate PrinterChamberApp::updateStateFromKnob(PB_SmartKnobState state)
{
    wanted_temperature_position = state.current_position;
    wanted_temperature = wanted_temperature_position * 5;

    // needed to next reload of App
    motor_config.position_nonce = wanted_temperature_position;
    motor_config.position = wanted_temperature_position;

    EntityStateUpdate new_state;

    new_state.entity_name = entity_name;
    new_state.new_value = wanted_temperature * 1.0;

    if (last_wanted_temperature != wanted_temperature)
    {
        last_wanted_temperature = wanted_temperature;
        new_state.changed = true;
    }

    return new_state;
}

void PrinterChamberApp::updateStateFromSystem(AppState state) {}

TFT_eSprite *PrinterChamberApp::render()
{
    uint32_t background = spr_->color565(0, 0, 0);
    uint32_t backgroun_light = spr_->color565(150, 150, 150);

    uint32_t color_dark_grey = spr_->color565(71, 71, 71);

    uint32_t color_red_bright = spr_->color565(255, 64, 0);
    uint32_t color_red_dark = spr_->color565(77, 20, 1); // 143, 36, 0

    uint32_t color_orange_bright = spr_->color565(255, 149, 0);
    uint32_t color_orange_dark = spr_->color565(77, 44, 1);

    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_v = TFT_WIDTH / 2;
    uint16_t screen_radius = TFT_WIDTH / 2;

    uint16_t footer_position = 190;

    spr_->fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, background);

    float range_radians = num_positions * motor_config.position_width_radians;
    float left_bound = PI / 2 + range_radians / 2;

    uint32_t dot_color;

    for (int i = 0; i < num_positions; i++)
    {

        if (i < wanted_temperature_position)
        {
            if (i * degrees_per_position < 80)
            {
                dot_color = TFT_WHITE;
            }
            else if (i * degrees_per_position < 100)
            {
                dot_color = color_orange_bright;
            }
            else
            {
                dot_color = color_red_bright;
            }
        }
        else
        {
            if (i * degrees_per_position < 80)
            {
                dot_color = color_dark_grey;
            }
            else if (i * degrees_per_position < 100)
            {
                dot_color = color_orange_dark;
            }
            else
            {
                dot_color = color_red_dark;
            }
        }

        float position_in_radians = range_radians / (num_positions - 1);

        float dot_position = left_bound - position_in_radians * i;

        spr_->fillTriangle(
            TFT_WIDTH / 2 + (screen_radius + 5) * cosf(dot_position - position_in_radians / 2.7),
            TFT_HEIGHT / 2 - (screen_radius + 5) * sinf(dot_position - position_in_radians / 2.7),
            TFT_WIDTH / 2 + (screen_radius + 5) * cosf(dot_position + position_in_radians / 2.7),
            TFT_HEIGHT / 2 - (screen_radius + 5) * sinf(dot_position + position_in_radians / 2.7),
            center_h,
            center_v,
            dot_color);
    }

    spr_->fillCircle(center_h, center_v, screen_radius - 40, background);

    uint8_t icon_size = 50;

    // spr_->fillRect(center_h - icon_size / 2, center_v - 70, icon_size, icon_size, TFT_YELLOW);
    uint32_t power_icon_color = color_dark_grey;

    if (current_temperature < wanted_temperature)
    {
        power_icon_color = TFT_YELLOW;
    }

    spr_->drawBitmap(center_h - icon_size / 2, center_v - 74, lightning_50, icon_size, icon_size, power_icon_color, background);

    spr_->setTextColor(TFT_WHITE);
    spr_->setFreeFont(&Roboto_Thin_Bold_24);
    sprintf(buf_, "Chamber", wanted_temperature);
    spr_->drawString(buf_, center_h, footer_position + 18, 1);

    // draw wanted temperature
    spr_->setFreeFont(&Roboto_Light_60);
    sprintf(buf_, "%d°C", wanted_temperature);
    spr_->drawString(buf_, center_h, center_v, 1);

    // draw current temperature
    spr_->setTextColor(TFT_WHITE);
    spr_->setFreeFont(&Roboto_Thin_24);
    sprintf(buf_, "%d°C", current_temperature);
    spr_->drawString(buf_, center_h, center_v + 50, 1);

    return this->spr_;
};