#pragma once

#if SK_DISPLAY

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <HTTPClient.h>

#include "logger.h"
#include "proto_gen/smartknob.pb.h"
#include "task.h"
#include "app_config.h"

#include "apps/icons.h"
#include "apps/apps.h"

#include "apps/menu.h"
#include "apps/music.h"
#include "apps/climate.h"

class DisplayTask : public Task<DisplayTask>
{
    friend class Task<DisplayTask>; // Allow base Task to invoke protected run()

public:
    DisplayTask(const uint8_t task_core);
    ~DisplayTask();

    QueueHandle_t getKnobStateQueue();

    void setBrightness(uint16_t brightness);
    void setLogger(Logger *logger);
    Apps *getApps();

protected:
    void run();

private:
    TFT_eSPI tft_ = TFT_eSPI();

    /** Full-size sprite used as a framebuffer */
    TFT_eSprite spr_ = TFT_eSprite(&tft_);
    Apps apps = Apps();

    QueueHandle_t app_state_queue_;

    AppConfig app_state_;
    SemaphoreHandle_t mutex_;
    uint16_t brightness_;
    Logger *logger_;
    void log(const char *msg);
};

#else

class DisplayTask
{
};

#endif
