#include "climate.h"

#include "music.h"

ClimateApp::ClimateApp(TFT_eSprite *spr_, std::string entity_name) : App(spr_)
{
    // TODO update this via some API
    current_temperature = 22;
    wanted_temperature = 25;

    this->entity_name = entity_name;

    // TODO, sync motor config with wanted temp on retrival

    motor_config = PB_SmartKnobConfig{
        wanted_temperature,
        0,
        wanted_temperature,
        16,
        35,
        8.225806452 * PI / 120,
        2,
        1,
        1.1,
        "SKDEMO_HVAC",
        0,
        {},
        0,
        27,
    };

    num_positions = motor_config.max_position - motor_config.min_position;
}

uint8_t ClimateApp::navigationNext()
{
    // back to menu
    return 0;
}

EntityStateUpdate ClimateApp::updateStateFromKnob(PB_SmartKnobState state)
{
    wanted_temperature = state.current_position;

    // needed to next reload of App
    motor_config.position_nonce = wanted_temperature;
    motor_config.position = wanted_temperature;

    adjusted_sub_position = state.sub_position_unit * state.config.position_width_radians;

    if (state.current_position == motor_config.min_position && state.sub_position_unit < 0)
    {
        adjusted_sub_position = -logf(1 - state.sub_position_unit * motor_config.position_width_radians / 5 / PI * 180) * 5 * PI / 180;
    }
    else if (state.current_position == motor_config.max_position && state.sub_position_unit > 0)
    {
        adjusted_sub_position = logf(1 + state.sub_position_unit * motor_config.position_width_radians / 5 / PI * 180) * 5 * PI / 180;
    }

    EntityStateUpdate new_state;

    new_state.entity_name = entity_name;
    new_state.new_value = wanted_temperature * 1.0;

    if (last_wanted_temperature != wanted_temperature)
    {
        last_wanted_temperature = wanted_temperature;
        new_state.changed = true;
        sprintf(new_state.app_slug, "%s", APP_SLUG_CLIMATE);
    }

    return new_state;
}

void ClimateApp::updateStateFromSystem(AppState state) {}

TFT_eSprite *ClimateApp::render()
{
    uint16_t DISABLED_COLOR = spr_->color565(71, 71, 71);

    // int32_t width = (wanted_temperature - motor_config.min_position) * TFT_WIDTH / (motor_config.max_position - motor_config.min_position);

    // TODO: make this real team, when sensor is connected

    uint32_t cooling_color = spr_->color565(80, 100, 200);
    uint32_t heating_color = spr_->color565(255, 128, 0);

    // draw division lines

    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_v = TFT_WIDTH / 2;
    uint16_t screen_radius = TFT_WIDTH / 2;

    float range_radians = (num_positions + 1) * motor_config.position_width_radians;

    float left_bound = PI / 2 + range_radians / 2;
    float right_bound = PI / 2 - range_radians / 2;

    spr_->drawLine(TFT_WIDTH / 2 + screen_radius * cosf(left_bound), TFT_HEIGHT / 2 - screen_radius * sinf(left_bound), TFT_WIDTH / 2 + (screen_radius - 10) * cosf(left_bound), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(left_bound), cooling_color);
    spr_->drawLine(TFT_WIDTH / 2 + screen_radius * cosf(right_bound), TFT_HEIGHT / 2 - screen_radius * sinf(right_bound), TFT_WIDTH / 2 + (screen_radius - 10) * cosf(right_bound), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(right_bound), heating_color);

    char buf_[16];

    uint32_t line_color;
    uint16_t tick_line_length = 10;

    for (int i = 1; i < num_positions - 1; i++)
    {
        if (motor_config.min_position + i < current_temperature)
        {
            line_color = cooling_color;
            tick_line_length = 10;
        }
        else if (motor_config.min_position + i == current_temperature)
        {
            line_color = TFT_WHITE;
            tick_line_length = 20;
        }
        else
        {
            line_color = heating_color;
            tick_line_length = 10;
        }
        float line_position = left_bound - (range_radians / num_positions) * i;
        spr_->drawLine(TFT_WIDTH / 2 + screen_radius * cosf(line_position), TFT_HEIGHT / 2 - screen_radius * sinf(line_position), TFT_WIDTH / 2 + (screen_radius - tick_line_length) * cosf(line_position), TFT_HEIGHT / 2 - (screen_radius - tick_line_length) * sinf(line_position), line_color);
    }

    // Draw min/max numbers
    float min_number_position = left_bound + (range_radians / num_positions) * 1.5;
    sprintf(buf_, "%d", motor_config.min_position);
    spr_->setTextColor(cooling_color);
    spr_->setFreeFont(&Roboto_Thin_24);
    spr_->drawString(buf_, TFT_WIDTH / 2 + (screen_radius - 15) * cosf(min_number_position), TFT_HEIGHT / 2 - (screen_radius - 15) * sinf(min_number_position), 1);

    float max_number_position = right_bound - (range_radians / num_positions) * 1.5;
    sprintf(buf_, "%d", motor_config.max_position);
    spr_->setTextColor(heating_color);
    spr_->setFreeFont(&Roboto_Thin_24);
    spr_->drawString(buf_, TFT_WIDTH / 2 + (screen_radius - 15) * cosf(max_number_position), TFT_HEIGHT / 2 - (screen_radius - 15) * sinf(max_number_position), 1);

    uint32_t snowflake_color = DISABLED_COLOR;
    uint32_t fire_color = DISABLED_COLOR;
    uint32_t wind_color = DISABLED_COLOR;

    // set the moving dot color
    uint32_t dot_color = TFT_WHITE;
    if (wanted_temperature < current_temperature)
    {
        dot_color = cooling_color;
    }
    else if (wanted_temperature > current_temperature)
    {
        dot_color = heating_color;
    }

    // TODO check for positions

    // draw current mode with text and color
    std::string status = "";
    if (wanted_temperature > current_temperature)
    {
        spr_->setTextColor(heating_color);
        fire_color = heating_color;
        status = "Heating";

        // draw arc of action
        float start_angle = left_bound - (range_radians / num_positions) * (current_temperature - motor_config.min_position);
        float wanted_angle = left_bound - (range_radians / num_positions) * (wanted_temperature - motor_config.min_position) - adjusted_sub_position;

        for (float r = start_angle; r >= wanted_angle; r -= 2 * PI / 180)
        {
            spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(r), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(r), 10, dot_color);
        }
    }
    else if (wanted_temperature == current_temperature)
    {
        spr_->setTextColor(TFT_WHITE);
        status = "idle";
        wind_color = TFT_GREENYELLOW;

        // draw arc of action

        float start_angle = left_bound - (range_radians / num_positions) * (current_temperature - motor_config.min_position);
        float wanted_angle = left_bound - (range_radians / num_positions) * (wanted_temperature - motor_config.min_position) - adjusted_sub_position;

        if (adjusted_sub_position < 0)
        {
            start_angle = left_bound - (range_radians / num_positions) * (wanted_temperature - motor_config.min_position) - adjusted_sub_position;
            wanted_angle = left_bound - (range_radians / num_positions) * (current_temperature - motor_config.min_position);
        }

        for (float r = start_angle; r >= wanted_angle; r -= 2 * PI / 180)
        {
            spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(r), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(r), 10, TFT_GREENYELLOW);
        }
    }
    else
    {
        spr_->setTextColor(cooling_color);
        snowflake_color = cooling_color;
        status = "Cooling";

        // draw arc of action
        // draw arc of action
        float start_angle = left_bound - (range_radians / num_positions) * (current_temperature - motor_config.min_position);
        float wanted_angle = left_bound - (range_radians / num_positions) * (wanted_temperature - motor_config.min_position) - adjusted_sub_position;

        for (float r = start_angle; r <= wanted_angle; r += 2 * PI / 180)
        {
            spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(r), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(r), 10, dot_color);
        }
    }

    // draw moving dot
    // if (num_positions > 0 && ((wanted_temperature == motor_config.min_position && state.sub_position_unit < 0) || (wanted_temperature == motor_config.max_position && state.sub_position_unit > 0)))
    // {

    //   spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(raw_angle), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(raw_angle), 5, dot_color);
    //   if (raw_angle < adjusted_angle)
    //   {
    //     for (float r = raw_angle; r <= adjusted_angle; r += 2 * PI / 180)
    //     {
    //       spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(r), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(r), 2, dot_color);
    //     }
    //     spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle), 2, dot_color);
    //   }
    //   else
    //   {
    //     for (float r = raw_angle; r >= adjusted_angle; r -= 2 * PI / 180)
    //     {
    //       spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(r), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(r), 2, dot_color);
    //     }
    //     spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle), 2, dot_color);
    //   }
    // }
    // else
    // {
    //   spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle), 5, dot_color);
    // }

    spr_->setFreeFont(&Roboto_Thin_24);
    spr_->drawString(status.c_str(), TFT_WIDTH / 2, TFT_HEIGHT / 2 - DESCRIPTION_Y_OFFSET - VALUE_OFFSET, 1);

    // draw wanted temperature
    spr_->setFreeFont(&Roboto_Light_60);
    sprintf(buf_, "%d°C", wanted_temperature);
    spr_->drawString(buf_, TFT_WIDTH / 2, TFT_HEIGHT / 2, 1);

    // draw current temperature
    spr_->setTextColor(TFT_WHITE);
    spr_->setFreeFont(&Roboto_Thin_24);
    sprintf(buf_, "%d°C", current_temperature);
    spr_->drawString(buf_, TFT_WIDTH / 2, TFT_HEIGHT / 2 + DESCRIPTION_Y_OFFSET + VALUE_OFFSET, 1);

    uint16_t center = TFT_WIDTH / 2;

    // draw bottom icons

    uint16_t icon_size = 20;
    uint16_t icon_margin = 3;

    spr_->drawBitmap(center - icon_size * 2 - icon_margin * 3, TFT_HEIGHT - 30, letter_A, icon_size, icon_size, TFT_WHITE, TFT_BLACK);
    spr_->drawBitmap(center - icon_size - icon_margin, TFT_HEIGHT - 30, snowflake, icon_size, icon_size, snowflake_color, TFT_BLACK);

    spr_->drawBitmap(center + icon_margin, TFT_HEIGHT - 30, fire, icon_size, icon_size, fire_color, TFT_BLACK);
    spr_->drawBitmap(center + icon_size + icon_margin * 3, TFT_HEIGHT - 30, wind, icon_size, icon_size, wind_color, TFT_BLACK);

    return this->spr_;
};