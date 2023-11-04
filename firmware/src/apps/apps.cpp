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

void Apps::update(AppState state)
{
    // TODO: update with AppState
    lock();
    char buf_[10];
    sprintf(buf_, "%d", active_id);
    // ESP_LOGD("apps.cpp", ">>> pre-updated");
    apps[buf_]->updateStateFromKnob(state.motor_state);
    // ESP_LOGD("apps.cpp", ">>> updated");

    unlock();
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