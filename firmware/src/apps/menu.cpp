#include "menu.h"

MenuApp::MenuApp(TFT_eSprite *spr_) : App(spr_)
{
    sprintf(room, "%s", "Office");

    motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        -1, // max position < min position indicates no bounds
        25 * PI / 180,
        2,
        1,
        0.55,
        "SKDEMO_Menu", // TODO: clean this
        0,
        {},
        0,
        200,
    };

    add_item(
        0,
        MenuItem{
            "3D printer",
            1, // TODO: make a const
            spr_->color565(0, 255, 200),
            printer_40,
            printer_80,
        });

    add_item(
        1,
        MenuItem{
            "Climate",
            1, // TODO: make a const
            spr_->color565(255, 128, 0),
            hvac_40,
            hvac_80,
        });

    add_item(
        2,
        MenuItem{
            "Shades",
            2, // TODO: make a const
            spr_->color565(66, 149, 245),
            shades_40,
            shades_80,
        });

    add_item(
        3,
        MenuItem{
            "Ceiling",
            3, // TODO: make a const
            spr_->color565(239, 223, 72),
            light_switch_40,
            light_switch_80,
        });

    add_item(
        4,
        MenuItem{
            "Workbench",
            4, // TODO: make a const
            spr_->color565(249, 165, 44),
            light_top_40,
            light_top_80,
        });

    add_item(
        5,
        MenuItem{
            "Music",
            5, // TODO: make a const
            spr_->color565(30, 215, 96),
            spotify_40,
            spotify_80,
        });

    // add_item(
    //     6,
    //     MenuItem{
    //         "Home Assistant",
    //         1, // TODO: make a const
    //         spr_->color565(24, 188, 242),
    //         home_assistant_40,
    //         home_assistant_80,
    //     });

    add_item(
        6,
        MenuItem{
            "Settings",
            1, // TODO: make a const
            spr_->color565(155, 95, 224),
            settings_40,
            settings_80,
        });
}

EntityStateUpdate MenuApp::updateStateFromKnob(PB_SmartKnobState state)
{
    // TODO: cache menu size

    int32_t position_for_menu_calc = state.current_position;

    // needed to next reload of App
    motor_config.position_nonce = position_for_menu_calc;
    motor_config.position = position_for_menu_calc;

    if (state.current_position < 0)
    {
        position_for_menu_calc = items.size() * 10000 + state.current_position;
    }

    current_menu_position = position_for_menu_calc % items.size();

    return EntityStateUpdate{};
}

void MenuApp::updateStateFromSystem(AppState state) {}

uint8_t MenuApp::navigationNext()
{
    return current_menu_position + 1; // +1 to shift from 0 position which is menu itself
}

TFT_eSprite *MenuApp::render()
{
    // ESP_LOGD("menu.cpp", "called real method");
    // return;
    // spr_->fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, TFT_ORANGE);

    // spr_->setTextColor(TFT_BLACK);
    // spr_->setFreeFont(&Roboto_Thin_24);
    // spr_->drawString("Menu", TFT_WIDTH / 2, TFT_HEIGHT / 2, 1);

    // current_menu_position = 0;

    MenuItem current_item = find_item(current_menu_position);
    MenuItem prev_item;
    MenuItem next_item;
    if (current_menu_position == 0)
    {
        prev_item = find_item(items.size() - 1);
        next_item = find_item(current_menu_position + 1);
    }
    else if (current_menu_position == items.size() - 1)
    {
        prev_item = find_item(current_menu_position - 1);
        next_item = find_item(0);
    }
    else
    {
        prev_item = find_item(current_menu_position - 1);
        next_item = find_item(current_menu_position + 1);
    }

    render_menu_screen(current_item, prev_item, next_item);

    return this->spr_;
}

std::string MenuApp::getClassName()
{
    return "App";
}

// TODO: add protection, could cause panic
MenuItem MenuApp::find_item(uint8_t id)
{
    return (*items.find(id)).second;
}

// TODO: add protection of overwriting same items
void MenuApp::add_item(uint8_t id, MenuItem item)
{
    // items[id] = item;
    items.insert(std::make_pair(id, item));
}

void MenuApp::render_menu_screen(MenuItem current, MenuItem prev, MenuItem next)
{
    uint32_t color_active = current.color;
    uint32_t color_inactive = spr_->color565(150, 150, 150);
    uint32_t label_color = color_inactive;
    uint32_t background = spr_->color565(0, 0, 0);

    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_v = TFT_WIDTH / 2;

    int8_t room_lable_w = 100;
    int8_t room_lable_h = 30;
    int8_t label_vertical_offset = 25;

    uint8_t icon_size_active = 80;
    uint8_t icon_size_inactive = 40;

    spr_->setTextDatum(CC_DATUM);

    spr_->fillRect(center_h - room_lable_w / 2, label_vertical_offset, room_lable_w, room_lable_h + 1, label_color); // +1 for height to draw circle right
    spr_->fillCircle(center_h - room_lable_w / 2, label_vertical_offset + room_lable_h / 2, room_lable_h / 2, label_color);
    spr_->fillCircle(center_h + room_lable_w / 2, label_vertical_offset + room_lable_h / 2, room_lable_h / 2, label_color);

    spr_->setTextColor(TFT_BLACK);
    spr_->setFreeFont(&Roboto_Thin_Bold_24);
    spr_->drawString(room, center_h, label_vertical_offset + room_lable_h / 2 - 1, 1);

    spr_->drawBitmap(center_h - icon_size_active / 2, center_v - icon_size_active / 2, current.big_icon, icon_size_active, icon_size_active, color_active, background);

    // left one
    spr_->drawBitmap(center_h - icon_size_active / 2 - 20 - icon_size_inactive, center_v - icon_size_inactive / 2, prev.small_icon, icon_size_inactive, icon_size_inactive, color_inactive, background);

    // right one
    spr_->drawBitmap(center_h + icon_size_active / 2 + 20, center_v - icon_size_inactive / 2, next.small_icon, icon_size_inactive, icon_size_inactive, color_inactive, background);

    spr_->setTextColor(color_active);
    spr_->setFreeFont(&Roboto_Thin_24);
    spr_->drawString(current.screen_name, center_h, center_v + icon_size_active / 2 + 30, 1);
};