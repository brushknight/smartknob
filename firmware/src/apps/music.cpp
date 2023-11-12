#include "music.h"

MusicApp::MusicApp(TFT_eSprite *spr_, std::string entity_name) : App(spr_)
{
    sprintf(author, "%s", "Beethoven");
    sprintf(track, "%s", "Moonlight Sonata");
    this->entity_name = entity_name;

    motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        20,
        230.0 / 20.0 * PI / 180,
        1,
        1,
        1.1,
        "SKDEMO_Music", // TODO: clean this
        0,
        {},
        0,
        90,
    };
}

uint8_t MusicApp::navigationNext()
{
    // back to menu
    return 0;
}

EntityStateUpdate MusicApp::updateStateFromKnob(PB_SmartKnobState state)
{
    current_volume_position = state.current_position;
    current_volume = current_volume_position * 5;

    // needed to next reload of App
    motor_config.position_nonce = current_volume_position;
    motor_config.position = current_volume_position;

    EntityStateUpdate new_state;

    new_state.entity_name = entity_name;
    new_state.new_value = current_volume * 1.0;

    if (last_volume != current_volume)
    {
        last_volume = current_volume;
        new_state.changed = true;
        sprintf(new_state.app_slug, "%s", APP_SLUG_MUSIC);
    }

    return new_state;
}

void MusicApp::updateStateFromSystem(AppState state) {}

TFT_eSprite *MusicApp::render()
{
    uint16_t DISABLED_COLOR = spr_->color565(71, 71, 71);

    uint32_t off_background = spr_->color565(0, 0, 0);
    uint32_t off_lamp_color = spr_->color565(150, 150, 150);

    uint32_t on_background = spr_->color565(71, 39, 1);
    uint32_t on_lamp_color = spr_->color565(245, 164, 66);

    uint32_t color_red_bright = spr_->color565(255, 64, 0);
    uint32_t color_red_dark = spr_->color565(143, 36, 0);

    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_v = TFT_WIDTH / 2;
    uint16_t screen_radius = TFT_WIDTH / 2;

    uint8_t icon_width = 80;
    uint8_t icon_height = 80;

    uint16_t offset_vertical = 50;

    uint16_t color_spotify = spr_->color565(30, 215, 96);

    spr_->fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, off_background);
    spr_->drawBitmap(center_h - icon_width / 2, center_v - icon_height / 2 - offset_vertical, spotify_80, icon_width, icon_height, color_spotify, off_background);

    spr_->setTextColor(TFT_WHITE);
    spr_->setFreeFont(&Roboto_Thin_Bold_24);
    spr_->drawString(author, center_h, center_v + icon_height / 2 - offset_vertical + 20, 1);

    spr_->setTextColor(TFT_WHITE);
    spr_->setFreeFont(&Roboto_Thin_20);
    spr_->drawString(track, center_h, center_v + icon_height / 2 - offset_vertical + 20 + 30, 1);

    uint8_t num_positions = 21;

    float range_radians = num_positions * motor_config.position_width_radians;
    float left_bound = PI / 2 + range_radians / 2;

    uint32_t dot_color;
    uint8_t dot_radius = 2;

    for (int i = 0; i < num_positions - 1; i++)
    {
        if (i < current_volume_position)
        {
            if (i < 15)
            {
                dot_color = TFT_WHITE;
            }
            else
            {
                dot_color = color_red_bright;
            }
            dot_radius = 4;
        }
        else
        {
            if (i < 15)
            {
                dot_color = DISABLED_COLOR;
            }
            else
            {
                dot_color = color_red_dark;
            }
            dot_radius = 2;
        }

        float dot_position = left_bound - (range_radians / (num_positions - 1)) * i;

        spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(dot_position), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(dot_position), dot_radius, dot_color);
    }

    uint16_t footer_position = 190;

    spr_->fillRect(0, footer_position, TFT_WIDTH, TFT_HEIGHT, off_lamp_color);

    icon_width = 44;
    icon_height = 30;

    uint16_t image_offset = 5;

    if (current_volume_position > 15)
    {
        spr_->drawBitmap(center_h - icon_width / 2 - 30, footer_position + image_offset, volume_30_4, icon_width, icon_height, color_red_dark, off_lamp_color);
    }
    else if (current_volume_position >= 10)
    {
        spr_->drawBitmap(center_h - icon_width / 2 - 30, footer_position + image_offset, volume_30_4, icon_width, icon_height, TFT_BLACK, off_lamp_color);
    }
    else if (current_volume_position >= 5)
    {
        spr_->drawBitmap(center_h - icon_width / 2 - 30, footer_position + image_offset, volume_30_3, icon_width, icon_height, TFT_BLACK, off_lamp_color);
    }
    else if (current_volume_position > 0)
    {
        spr_->drawBitmap(center_h - icon_width / 2 - 30, footer_position + image_offset, volume_30_2, icon_width, icon_height, TFT_BLACK, off_lamp_color);
    }
    else
    {
        spr_->drawBitmap(center_h - icon_width / 2 - 30, footer_position + image_offset, volume_30_1, icon_width, icon_height, DISABLED_COLOR, off_lamp_color);
    }

    spr_->setTextColor(TFT_BLACK);
    spr_->setFreeFont(&Roboto_Thin_Bold_24);

    char buf_[4];
    sprintf(buf_, "%d%%", current_volume);
    spr_->drawString(buf_, center_h + 30, footer_position + 18, 1);

    return this->spr_;
};