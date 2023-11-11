#include "apps.h"
#include "menu.h"

#include <typeinfo>

Apps::Apps()
{
    mutex = xSemaphoreCreateMutex();
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
    // ESP_LOGD("apps.cpp", ">>> pre-rendered");
    TFT_eSprite *spr_ = apps[buf_]->render();
    // apps[buf_]->render();
    // ESP_LOGD("apps.cpp", ">>> rendered");

    unlock();
    return spr_;
}

void Apps::setActive(uint8_t id)
{
    lock();
    active_id = id;
    unlock();
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