#include "light_dimmer.h"

LightDimmerApp::LightDimmerApp(TFT_eSprite *spr_, std::string entity_name) : App(spr_)
{
    // sprintf(author, "%s", "Beethoven");
    // sprintf(track, "%s", "Moonlight Sonata");
    this->entity_name = entity_name;

    motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        100,
        3.6 / 2 * PI / 180,
        1,
        1,
        1.1,
        "SKDEMO_Light_dimmer",
        0,
        {},
        0,
        27,
    };
}

uint8_t LightDimmerApp::navigationNext()
{
    // back to menu
    return 0;
}

EntityStateUpdate LightDimmerApp::updateStateFromKnob(PB_SmartKnobState state)
{
    current_position = state.current_position;
    sub_position_unit = state.sub_position_unit;
    // // needed to next reload of App
    motor_config.position_nonce = current_position;
    motor_config.position = current_position;

    adjusted_sub_position = sub_position_unit * motor_config.position_width_radians;

    if (state.current_position == motor_config.min_position && sub_position_unit < 0)
    {
        adjusted_sub_position = -logf(1 - sub_position_unit * motor_config.position_width_radians / 5 / PI * 180) * 5 * PI / 180;
    }
    else if (state.current_position == motor_config.max_position && sub_position_unit > 0)
    {
        adjusted_sub_position = logf(1 + sub_position_unit * motor_config.position_width_radians / 5 / PI * 180) * 5 * PI / 180;
    }

    EntityStateUpdate new_state;

    new_state.entity_name = entity_name;
    new_state.new_value = current_position * 1.0;

    if (last_position != current_position)
    {
        last_position = current_position;
        new_state.changed = true;
        sprintf(new_state.app_slug, "%s", APP_SLUG_LIGHT_DIMMER);
    }

    return new_state;
}

void LightDimmerApp::updateStateFromSystem(AppState state) {}

TFT_eSprite *LightDimmerApp::render()
{
    uint16_t DISABLED_COLOR = spr_->color565(71, 71, 71);

    uint16_t screen_radius = TFT_WIDTH / 2;

    float left_bound = PI / 2;
    float right_bound = 0;
    float range_radians = (motor_config.max_position - motor_config.min_position) * motor_config.position_width_radians;

    left_bound = PI / 2;
    right_bound = PI / 2 - range_radians - motor_config.position_width_radians;

    float raw_angle = left_bound - (current_position - motor_config.min_position) * motor_config.position_width_radians;
    float adjusted_angle = raw_angle - adjusted_sub_position;

    uint32_t off_background = spr_->color565(0, 0, 0);
    uint32_t off_lamp_color = spr_->color565(150, 150, 150);

    uint32_t on_background = spr_->color565(71, 39, 1);
    uint32_t on_lamp_color = spr_->color565(245, 164, 66);

    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_v = TFT_WIDTH / 2;

    uint8_t icon_size = 80;

    uint16_t offset_vertical = 30;

    char buf_[6];
    sprintf(buf_, "%d%%", current_position);

    float start_angle = left_bound;
    float wanted_angle = right_bound;

    if (current_position == 0)
    {
        spr_->fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, off_background);
        spr_->drawBitmap(center_h - icon_size / 2, center_v - icon_size / 2 - offset_vertical, lamp_regular, icon_size, icon_size, off_lamp_color, off_background);
        spr_->setTextColor(off_lamp_color);
        spr_->setFreeFont(&Roboto_Thin_24);
        spr_->drawString("Kitchen", center_h, center_v + icon_size / 2 + 30 - offset_vertical, 1);
        spr_->drawString("off", center_h, center_v + icon_size / 2 + 60 - offset_vertical, 1);

        // draw dot movong path
        for (float r = start_angle; r >= wanted_angle; r -= 2 * PI / 180)
        {
            spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(r), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(r), 10, DISABLED_COLOR);
        }
    }
    else
    {
        spr_->fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, on_background);
        spr_->drawBitmap(center_h - icon_size / 2, center_v - icon_size / 2 - offset_vertical, lamp_solid, icon_size, icon_size, on_lamp_color, on_background);
        spr_->setTextColor(on_lamp_color);
        spr_->setFreeFont(&Roboto_Thin_24);
        spr_->drawString("Kitchen", center_h, center_v + icon_size / 2 + 30 - offset_vertical, 1);
        spr_->drawString(buf_, center_h, center_v + icon_size / 2 + 60 - offset_vertical, 1);

        // draw dot movong path
        for (float r = start_angle; r >= wanted_angle; r -= 2 * PI / 180)
        {
            spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(r), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(r), 10, DISABLED_COLOR);
        }

        wanted_angle = left_bound - (range_radians / num_positions) * (current_position - motor_config.min_position) - adjusted_sub_position - motor_config.position_width_radians;
        if (wanted_angle < right_bound - motor_config.position_width_radians)
        {
            wanted_angle = right_bound - motor_config.position_width_radians;
        }
        for (float r = start_angle; r >= wanted_angle; r -= 2 * PI / 180)
        {
            spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(r), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(r), 10, on_lamp_color);
        }
    }

    // set the moving dot color
    uint32_t dot_color = TFT_WHITE;

    if (current_position < 1)
    {
        dot_color = off_background;
    }
    else
    {
        dot_color = on_background;
    }

    // draw moving dot
    if (num_positions > 0 && ((current_position == motor_config.min_position && sub_position_unit < 0) || (current_position == motor_config.max_position && sub_position_unit > 0)))
    {

        if (adjusted_angle > left_bound)
        {
            adjusted_angle = left_bound;
        }
        else if (adjusted_angle < right_bound)
        {
            adjusted_angle = right_bound;
        }

        spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle), 5, dot_color);
    }
    else
    {
        spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle), 5, dot_color);
    }

    return this->spr_;
};