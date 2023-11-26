#pragma once
#include "apps.h"
#include "menu.h"

#include <typeinfo>

Apps::Apps()
{
    mutex = xSemaphoreCreateMutex();
}

void Apps::setSprite(TFT_eSprite *spr_)
{
    this->spr_ = spr_;
}

void Apps::add(uint8_t id, App *app)
{
    lock();
    char buf_[10];
    sprintf(buf_, "%d", id);

    // MenuApp *app = new MenuApp(spr_);

    apps.insert(std::make_pair(buf_, app));
    // ESP_LOGD("apps.cpp", ">>> inserted menu App");

    // apps.insert(apps.begin() + id, std::move(app), std::move(app));
    unlock();
}

EntityStateUpdate Apps::update(AppState state)
{
    // TODO: update with AppState
    lock();
    char buf_[10];
    sprintf(buf_, "%d", active_id);
    // ESP_LOGD("apps.cpp", ">>> pre-updated");
    EntityStateUpdate new_state_update = apps[buf_]->updateStateFromKnob(state.motor_state);
    apps[buf_]->updateStateFromSystem(state);
    // ESP_LOGD("apps.cpp", ">>> updated");

    unlock();
    return new_state_update;
}

TFT_eSprite *Apps::renderActive()
{
    // TODO: update with AppState
    lock();
    char buf_[10];
    sprintf(buf_, "%d", active_id);
    if (apps[buf_] == nullptr)
    {
        ESP_LOGE("apps.cpp", "null pointer instead of app");
        unlock();
        return spr_;
    }
    TFT_eSprite *spr_ = apps[buf_]->render();

    unlock();
    return spr_;
}

void Apps::setActive(uint8_t id)
{
    lock();
    active_id = id;
    unlock();
}

// settings and menu apps kept aside for a reason. We will add them manually later
void Apps::loadApp(uint8_t position, std::string app_slug, std::string entity_id, char entity_name[32])
{
    ESP_LOGD("apps.cpp", "loading app %d %s %s %s", position, app_slug.c_str(), entity_id.c_str(), entity_name);
    if (app_slug.compare(APP_SLUG_CLIMATE) == 0)
    {
        ClimateApp *app = new ClimateApp(this->spr_, entity_id);
        add(position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug.c_str(), entity_id.c_str(), entity_name);
    }
    else if (app_slug.compare(APP_SLUG_3D_PRINTER) == 0)
    {
        PrinterChamberApp *app = new PrinterChamberApp(this->spr_, entity_id);
        add(position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug.c_str(), entity_id.c_str(), entity_name);
    }
    else if (app_slug.compare(APP_SLUG_BLINDS) == 0)
    {
        BlindsApp *app = new BlindsApp(this->spr_, entity_id);
        add(position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug.c_str(), entity_id.c_str(), entity_name);
    }
    else if (app_slug.compare(APP_SLUG_LIGHT_DIMMER) == 0)
    {
        LightDimmerApp *app = new LightDimmerApp(this->spr_, entity_id);
        add(position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug.c_str(), entity_id.c_str(), entity_name);
    }
    else if (app_slug.compare(APP_SLUG_LIGHT_SWITCH) == 0)
    {
        LightSwitchApp *app = new LightSwitchApp(this->spr_, entity_id);
        add(position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug.c_str(), entity_id.c_str(), entity_name);
    }
    else if (app_slug.compare(APP_SLUG_MUSIC) == 0)
    {
        MusicApp *app = new MusicApp(this->spr_, entity_id);
        add(position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug.c_str(), entity_id.c_str(), entity_name);
    }
}

uint8_t Apps::navigationNext()
{
    lock();
    char buf_[10];
    sprintf(buf_, "%d", active_id);
    // ESP_LOGD("apps.cpp", ">>> pre-updated");
    uint8_t next = apps[buf_]->navigationNext();
    // ESP_LOGD("apps.cpp", ">>> updated");
    unlock();
    return next;
}

PB_SmartKnobConfig Apps::getActiveMotorConfig()
{
    lock();
    char buf_[10];
    sprintf(buf_, "%d", active_id);
    // ESP_LOGD("apps.cpp", ">>> pre-updated");
    PB_SmartKnobConfig motor_config = apps[buf_]->getMotorConfig();
    // ESP_LOGD("apps.cpp", ">>> updated");

    unlock();
    return motor_config;
}

// App *Apps::find(uint8_t id)
// {
//     // TODO: add protection with array size
//     return apps[id];
// }

void Apps::lock()
{
    xSemaphoreTake(mutex, portMAX_DELAY);
}
void Apps::unlock()
{
    xSemaphoreGive(mutex);
}