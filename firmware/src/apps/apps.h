#pragma once

#include <map>

#include "app.h"
#include "../app_config.h"

class Apps
{
public:
    Apps();
    void add(uint8_t id, App *app);
    void update(AppState state);
    TFT_eSprite *renderActive();
    void setActive(uint8_t id);

private:
    QueueHandle_t mutex;
    std::map<std::string, std::shared_ptr<App>> apps;
    // std::vector<std::unique_ptr<App>> apps;
    uint8_t active_id;

    // App *find(uint8_t id);
    void lock();
    void unlock();
};
// map of apps
// keep state of all apps
// keep active app
// allow app update
// allow app render
// protect read/write with mutex due to thread sharing