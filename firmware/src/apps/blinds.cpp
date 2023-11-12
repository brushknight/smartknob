#include "blinds.h"

BlindsApp::BlindsApp(TFT_eSprite *spr_, std::string entity_name) : App(spr_)
{
    this->entity_name = entity_name;

    motor_config = PB_SmartKnobConfig{
        15,
        0,
        15,
        0,
        20,
        8.225806452 * PI / 120,
        2,
        1,
        1.1,
        "SKDEMO_Shades",
        0,
        {},
        0,
        27,
    };
}

uint8_t BlindsApp::navigationNext()
{
    // back to menu
    return 0;
}

EntityStateUpdate BlindsApp::updateStateFromKnob(PB_SmartKnobState state)
{
    current_closed_position = state.current_position;

    // needed to next reload of App
    motor_config.position_nonce = current_closed_position;
    motor_config.position = current_closed_position;

    EntityStateUpdate new_state;

    new_state.entity_name = entity_name;
    new_state.new_value = current_closed_position * 5.0; // 5 percent per position

    if (last_closed_position != current_closed_position)
    {
        last_closed_position = current_closed_position;
        new_state.changed = true;
        sprintf(new_state.app_slug, "%s", APP_SLUG_BLINDS);
    }

    return new_state;
}

void BlindsApp::updateStateFromSystem(AppState state) {}

TFT_eSprite *BlindsApp::render()
{
    uint16_t DISABLED_COLOR = spr_->color565(71, 71, 71);

    uint32_t light_background = spr_->color565(150, 150, 150);

    uint32_t shade_bar_color = TFT_OLIVE;
    uint32_t shade_color = spr_->color565(50, 50, 50);

    spr_->fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, light_background);
    int32_t height = (current_closed_position - motor_config.min_position) * TFT_HEIGHT / (motor_config.max_position - motor_config.min_position);
    spr_->fillRect(0, 0, TFT_WIDTH, height, shade_color);
    spr_->fillRect(0, height - 10, TFT_WIDTH, 10, shade_bar_color);

    spr_->setFreeFont(&Roboto_Light_60);

    uint16_t center = TFT_WIDTH / 2;

    uint8_t arrow_size = 50;

    if (current_closed_position == 0)
    {
        sprintf(buf_, "%s", "Opened");
        spr_->drawBitmap(center - arrow_size / 2, TFT_HEIGHT - 20 - arrow_size, arrow_down_50, arrow_size, arrow_size, shade_bar_color, light_background);
    }
    else if (current_closed_position == 10)
    {
        sprintf(buf_, "%s", "Half");
    }
    else if (current_closed_position == 20)
    {
        sprintf(buf_, "%s", "Closed");
        spr_->drawBitmap(center - arrow_size / 2, TFT_HEIGHT - 20 - arrow_size, arrow_up_50, arrow_size, arrow_size, shade_bar_color, shade_color);
    }
    else
    {
        sprintf(buf_, "%d%%", current_closed_position * 5);
        // spr_->drawBitmap(center - arrow_size / 2, TFT_HEIGHT - 20 - arrow_size, arrow_up_50, arrow_size, arrow_size, TFT_OLIVE, TFT_TRANSPARENT);
    }

    spr_->drawString(buf_, TFT_WIDTH / 2, TFT_HEIGHT / 2, 1);

    // spr_->drawNumber(current_closed_position, TFT_WIDTH / 2, TFT_HEIGHT / 2, 1);
    spr_->setTextColor(TFT_WHITE);
    spr_->setFreeFont(&Roboto_Thin_24);
    spr_->drawString("Bedroom shade", TFT_WIDTH / 2, TFT_HEIGHT / 2 - DESCRIPTION_Y_OFFSET - VALUE_OFFSET, 1);

    return this->spr_;
};