#include "light_switch.h"

LightSwitchApp::LightSwitchApp(TFT_eSprite *spr_) : App(spr_)
{
    // sprintf(author, "%s", "Beethoven");
    // sprintf(track, "%s", "Moonlight Sonata");

    motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        1,
        60 * PI / 180,
        1,
        1,
        0.55, // Note the snap point is slightly past the midpoint (0.5); compare to normal detents which use a snap point *past* the next value (i.e. > 1)
        "SKDEMO_Light_switch",
        0,
        {},
        0,
        27,
    };
}

uint8_t LightSwitchApp::navigationNext()
{
    // back to menu
    return 0;
}

void LightSwitchApp::updateStateFromKnob(PB_SmartKnobState state)
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
}

void LightSwitchApp::updateStateFromSystem(AppState state) {}

TFT_eSprite *LightSwitchApp::render()
{
    uint16_t DISABLED_COLOR = spr_->color565(71, 71, 71);

    uint32_t off_background = spr_->color565(0, 0, 0);
    uint32_t off_lamp_color = spr_->color565(150, 150, 150);

    uint32_t on_background = spr_->color565(71, 39, 1);
    uint32_t on_lamp_color = spr_->color565(245, 164, 66);

    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_v = TFT_WIDTH / 2;

    uint8_t icon_size = 80;

    uint16_t offset_vertical = 20;
    uint16_t screen_radius = TFT_WIDTH / 2;

    float left_bound = PI / 2;
    float right_bound = 0;
    float range_radians = (motor_config.max_position - motor_config.min_position) * motor_config.position_width_radians;

    left_bound = PI / 2 + range_radians / 2;
    right_bound = PI / 2 - range_radians / 2;

    float raw_angle = left_bound - (current_position - motor_config.min_position) * motor_config.position_width_radians;
    float adjusted_angle = raw_angle - adjusted_sub_position;

    if (current_position == 0)
    {
        spr_->fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, off_background);
        spr_->drawBitmap(center_h - icon_size / 2, center_v - icon_size / 2 - offset_vertical, lamp_regular, icon_size, icon_size, off_lamp_color, off_background);
        spr_->setTextColor(off_lamp_color);
        spr_->setFreeFont(&Roboto_Thin_24);
        spr_->drawString("Kitchen", center_h, center_v + icon_size / 2 + 30 - offset_vertical, 1);
    }
    else
    {
        spr_->fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, on_background);
        spr_->drawBitmap(center_h - icon_size / 2, center_v - icon_size / 2 - offset_vertical, lamp_solid, icon_size, icon_size, on_lamp_color, on_background);
        spr_->setTextColor(on_lamp_color);
        spr_->setFreeFont(&Roboto_Thin_24);
        spr_->drawString("Kitchen", center_h, center_v + icon_size / 2 + 30 - offset_vertical, 1);
    }

    // set the moving dot color
    uint32_t dot_color = TFT_WHITE;

    if (current_position < 1)
    {
        dot_color = off_lamp_color;
    }
    else
    {
        dot_color = on_lamp_color;
    }

    // draw moving dot
    if (num_positions > 0 && ((current_position == motor_config.min_position && sub_position_unit < 0) || (current_position == motor_config.max_position && sub_position_unit > 0)))
    {

        spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(raw_angle), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(raw_angle), 5, dot_color);
        if (raw_angle < adjusted_angle)
        {
            for (float r = raw_angle; r <= adjusted_angle; r += 2 * PI / 180)
            {
                spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(r), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(r), 2, dot_color);
            }
            spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle), 2, dot_color);
        }
        else
        {
            for (float r = raw_angle; r >= adjusted_angle; r -= 2 * PI / 180)
            {
                spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(r), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(r), 2, dot_color);
            }
            spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle), 2, dot_color);
        }
    }
    else
    {
        spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle), 5, dot_color);
    }

    return this->spr_;
};