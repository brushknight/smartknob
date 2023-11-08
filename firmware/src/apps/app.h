#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "../proto_gen/smartknob.pb.h"
#include "../app_config.h"
#include "icons.h"

const uint32_t APP_ID_CLIMATE = 1;
const uint32_t APP_ID_SHADES = 2;
// const uint32_t APP_ID_HOME_ASSISTANT = 6;

class App
{
public:
    App(TFT_eSprite *spr_)
    {
        this->spr_ = spr_;
    }
    virtual ~App() {}
    // virtual uint8_t getAppId();
    virtual TFT_eSprite *render();
    // {
    //     ESP_LOGD("app.h", "called stub method");
    //     return;
    //     spr_->fillCircle(TFT_WIDTH / 2, TFT_HEIGHT / 2, 10, TFT_RED);
    //     // return spr_;
    // }
    virtual void updateStateFromKnob(PB_SmartKnobState state);
    virtual void updateStateFromSystem(AppState state);

    PB_SmartKnobConfig getMotorConfig()
    {
        return motor_config;
    }
    // get motor config (with wanted position)

    std::string getClassName()
    {
        return "App";
    }

    virtual uint8_t navigationNext();

protected:
    /** Full-size sprite used as a framebuffer */
    TFT_eSprite *spr_;
    PB_SmartKnobConfig motor_config;
};