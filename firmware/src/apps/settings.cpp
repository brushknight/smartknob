#include "settings.h"

SettingsApp::SettingsApp(TFT_eSprite *spr_) : App(spr_)
{
    sprintf(fw_version, "FW: %s", "v0.1-demo");
    sprintf(room, "%s", "Kitchen");

    motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        10,
        90 * PI / 180,
        1,
        1,
        0.55,
        "SKDEMO_Settings",
        0,
        {},
        0,
        70,
    };

    num_positions = motor_config.max_position - motor_config.min_position;
}

uint8_t SettingsApp::navigationNext()
{
    // back to menu
    return 0;
}

EntityStateUpdate SettingsApp::updateStateFromKnob(PB_SmartKnobState state)
{
    current_position = state.current_position;

    // // needed to next reload of App
    motor_config.position_nonce = current_position;
    motor_config.position = current_position;

    adjusted_sub_position = state.sub_position_unit * state.config.position_width_radians;

    if (state.current_position == motor_config.min_position && state.sub_position_unit < 0)
    {
        adjusted_sub_position = -logf(1 - state.sub_position_unit * motor_config.position_width_radians / 5 / PI * 180) * 5 * PI / 180;
    }
    else if (state.current_position == motor_config.max_position && state.sub_position_unit > 0)
    {
        adjusted_sub_position = logf(1 + state.sub_position_unit * motor_config.position_width_radians / 5 / PI * 180) * 5 * PI / 180;
    }

    return EntityStateUpdate{};
}

void SettingsApp::updateStateFromSystem(AppState state)
{
    connectivity_state.ip_address = state.connectivity_state.ip_address;
    connectivity_state.is_connected = state.connectivity_state.is_connected;
    connectivity_state.signal_strength = state.connectivity_state.signal_strength;
    connectivity_state.signal_strenth_status = state.connectivity_state.signal_strenth_status;
    connectivity_state.ssid = state.connectivity_state.ssid;

    // current_volume = current_volume_position * 5;

    // // needed to next reload of App
    // motor_config.position_nonce = current_volume_position;
    // motor_config.position = current_volume_position;
}

TFT_eSprite *SettingsApp::render()
{
    uint16_t DISABLED_COLOR = spr_->color565(71, 71, 71);

    float range_radians = (num_positions + 1) * motor_config.position_width_radians;

    float left_bound = PI / 2 + range_radians / 2;
    float right_bound = PI / 2 - range_radians / 2;

    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_v = TFT_WIDTH / 2;

    uint16_t screen_radius = TFT_WIDTH / 2;

    float raw_angle = left_bound - (current_position - motor_config.min_position) * motor_config.position_width_radians;
    float adjusted_angle = raw_angle - adjusted_sub_position;

    if (current_position == 0)
    {
        // spr_->fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, TFT_PURPLE);

        spr_->setTextColor(TFT_WHITE);
        spr_->setFreeFont(&Roboto_Thin_24);
        spr_->drawString("Room", center_h, 20, 1);

        spr_->setTextColor(TFT_WHITE);
        spr_->setFreeFont(&Roboto_Thin_24);
        spr_->drawString(room, center_h, 50, 1);

        // draw line to separate title
        spr_->fillRect(0, 70, TFT_WIDTH, 2, DISABLED_COLOR);

        uint8_t wifi_icon_size = 40;
        uint32_t wifi_icon_color = TFT_WHITE;
        if (!connectivity_state.is_connected)
        {
            wifi_icon_color = DISABLED_COLOR;
        }

        spr_->drawBitmap(20, 75, wifi_40, wifi_icon_size, wifi_icon_size, wifi_icon_color, TFT_BLACK);

        // wifi_30

        // log(connectivity_state.ssid.c_str());

        std::string ip = connectivity_state.ip_address;
        std::string SSID = connectivity_state.ssid;
        int16_t signal_strength = connectivity_state.signal_strength;
        std::string signal_strength_text = "";

        uint32_t signal_strength_color = TFT_GREEN;

        switch (connectivity_state.signal_strenth_status)
        {
        case 0:
            signal_strength_color = TFT_GREEN;
            signal_strength_text = "Excellent";
            break;
        case 1:
            signal_strength_color = TFT_YELLOW;
            signal_strength_text = "Good";
            break;
        case 2:
            signal_strength_color = TFT_ORANGE;
            signal_strength_text = "Fair";
            break;
        case 3:
            signal_strength_color = TFT_RED;
            signal_strength_text = "Poor";
            break;
        case 4:
            signal_strength_color = TFT_RED;
            signal_strength_text = "No signal";
            break;
        default:
            break;
        }

        char buf_[48];

        if (connectivity_state.is_connected)
        {
            spr_->setTextColor(signal_strength_color);
            spr_->setFreeFont(&Roboto_Thin_20);
            spr_->drawString(signal_strength_text.c_str(), center_h, 94, 1);

            spr_->setTextColor(TFT_WHITE);
            spr_->setFreeFont(&Roboto_Thin_20);
            sprintf(buf_, "%ddb", signal_strength);
            spr_->drawString(buf_, center_h + 75, 94, 1);

            spr_->setTextColor(TFT_WHITE);
            spr_->setFreeFont(&Roboto_Thin_20);
            sprintf(buf_, "SSID: %s", SSID.c_str());
            spr_->drawString(buf_, center_h, 130, 1);

            spr_->setTextColor(TFT_WHITE);
            spr_->setFreeFont(&Roboto_Thin_20);
            sprintf(buf_, "ip: %s", ip.c_str());
            spr_->drawString(buf_, center_h, 160, 1);
        }

        spr_->setTextColor(TFT_WHITE);
        spr_->setFreeFont(&Roboto_Thin_20);
        spr_->drawString(fw_version, center_h, 190, 1);
    }
    else if (current_position == 1)
    {
        // ben10 easter egg
        uint32_t backroung_green = TFT_GREENYELLOW;
        uint32_t center_margin = 20;
        spr_->fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, backroung_green);
        spr_->fillTriangle(0, 0, TFT_WIDTH / 2 - center_margin / 2, center_v, 0, TFT_HEIGHT, TFT_BLACK);
        spr_->fillTriangle(TFT_WIDTH, 0, TFT_WIDTH / 2 + center_margin / 2, center_v, TFT_WIDTH, TFT_HEIGHT, TFT_BLACK);

        uint8_t black_border = 20;
        spr_->fillTriangle(0, 0 + black_border, TFT_WIDTH / 2 - center_margin / 2 - black_border, center_v, 0, TFT_HEIGHT - black_border, DISABLED_COLOR);
        spr_->fillTriangle(TFT_WIDTH, 0 + black_border, TFT_WIDTH / 2 + center_margin / 2 + black_border, center_v, TFT_WIDTH, TFT_HEIGHT - black_border, DISABLED_COLOR);

        for (float r = 0; r >= -6.3; r -= 2 * PI / 180)
        {
            spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(r), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(r), 11, TFT_BLACK);
        }

        if (adjusted_angle > left_bound)
        {
            adjusted_angle = left_bound;
        }
        else if (adjusted_angle < right_bound)
        {
            adjusted_angle = right_bound;
        }

        spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle), 5, backroung_green);
        spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle - PI / 2), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle - PI / 2), 5, backroung_green);
        spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle - 2 * PI / 2), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle - 2 * PI / 2), 5, backroung_green);
        spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle - 3 * PI / 2), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle - 3 * PI / 2), 5, backroung_green);
    }
    else
    {
        // ben10 easter egg
        uint32_t ben_10_green = TFT_GREENYELLOW;

        uint8_t triangle_offset_h = 20;
        uint8_t triangle_offset_v = 40;
        uint32_t backroung_grey = spr_->color565(0, 0, 0);
        spr_->fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, ben_10_green);

        uint32_t icon_width = 75;
        uint32_t icon_height = 174;

        if (current_position == 2)
        {
            spr_->drawBitmap(center_h - icon_width / 2, center_v - icon_height / 2, omnitrix_char_1, icon_width, icon_height, TFT_BLACK, ben_10_green);
        }
        else if (current_position == 3)
        {
            icon_width = 85;
            spr_->drawBitmap(center_h - icon_width / 2, center_v - icon_height / 2, omnitrix_char_2, icon_width, icon_height, TFT_BLACK, ben_10_green);
        }

        spr_->fillTriangle(0, 0, center_h + triangle_offset_h, 0, 0, center_v + triangle_offset_v, TFT_BLACK);
        spr_->fillTriangle(TFT_WIDTH, 0, center_h - triangle_offset_h, 0, TFT_WIDTH, center_v + triangle_offset_v, TFT_BLACK);

        spr_->fillTriangle(0, TFT_HEIGHT, center_h + triangle_offset_h, TFT_HEIGHT, 0, center_v - triangle_offset_v, TFT_BLACK);
        spr_->fillTriangle(TFT_WIDTH, TFT_HEIGHT, center_h - triangle_offset_h, TFT_HEIGHT, TFT_WIDTH, center_v - triangle_offset_v, TFT_BLACK);

        // spr_->fillTriangle(center_h, triangle_offset_v, TFT_WIDTH - triangle_offset_h, center_v, triangle_offset_h, center_v, ben_10_green);
        // spr_->fillTriangle(center_h, TFT_HEIGHT - triangle_offset_v, TFT_WIDTH - triangle_offset_h, center_v, triangle_offset_h, center_v, ben_10_green);

        // draw controls

        for (float r = 0; r >= -6.3; r -= 2 * PI / 180)
        {
            spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(r), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(r), 11, TFT_BLACK);
        }

        if (adjusted_angle > left_bound)
        {
            adjusted_angle = left_bound;
        }
        else if (adjusted_angle < right_bound)
        {
            adjusted_angle = right_bound;
        }

        spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle), 5, ben_10_green);
        spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle - PI / 2), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle - PI / 2), 5, ben_10_green);
        spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle - 2 * PI / 2), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle - 2 * PI / 2), 5, ben_10_green);
        spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle - 3 * PI / 2), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle - 3 * PI / 2), 5, ben_10_green);
    }
    return this->spr_;
};