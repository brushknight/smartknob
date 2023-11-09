#if SK_LEDS
#include <FastLED.h>
#endif

#if SK_STRAIN
#include <HX711.h>
#endif

#if SK_ALS
#include <Adafruit_VEML7700.h>
#endif

#include "app_task.h"
#include "semaphore_guard.h"
#include "util.h"

#if SK_LEDS
CRGB leds[NUM_LEDS];
#endif

#if SK_STRAIN
HX711 scale;
#endif

#if SK_ALS
Adafruit_VEML7700 veml = Adafruit_VEML7700();
#endif

AppTask::AppTask(const uint8_t task_core, MotorTask &motor_task, DisplayTask *display_task) : Task("App", 3400, 1, task_core),
                                                                                              stream_(),
                                                                                              motor_task_(motor_task),
                                                                                              display_task_(display_task),
                                                                                              plaintext_protocol_(stream_, [this]()
                                                                                                                  { motor_task_.runCalibration(); }),
                                                                                              proto_protocol_(stream_, [this](PB_SmartKnobConfig &config)
                                                                                                              { applyConfig(config, true); })
{
#if SK_DISPLAY
    assert(display_task != nullptr);
#endif

    log_queue_ = xQueueCreate(10, sizeof(std::string *));
    assert(log_queue_ != NULL);

    knob_state_queue_ = xQueueCreate(1, sizeof(PB_SmartKnobState));
    assert(knob_state_queue_ != NULL);

    connectivity_status_queue_ = xQueueCreate(1, sizeof(ConnectivityState));
    assert(connectivity_status_queue_ != NULL);

    mutex_ = xSemaphoreCreateMutex();
    assert(mutex_ != NULL);
}

AppTask::~AppTask()
{
    vSemaphoreDelete(mutex_);
}

void AppTask::setApps(Apps *apps)
{
    this->apps = apps;
}

void AppTask::run()
{
    stream_.begin();

#if SK_LEDS
    FastLED.addLeds<SK6812, PIN_LED_DATA, GRB>(leds, NUM_LEDS);
#endif

#if SK_ALS && PIN_SDA >= 0 && PIN_SCL >= 0
    Wire.begin(PIN_SDA, PIN_SCL);
    Wire.setClock(400000);
#endif
#if SK_STRAIN
    scale.begin(STRAIN_DO, STRAIN_SCK);
#endif

#if SK_ALS
    if (veml.begin())
    {
        veml.setGain(VEML7700_GAIN_2);
        veml.setIntegrationTime(VEML7700_IT_400MS);
    }
    else
    {
        log("ALS sensor not found!");
    }
#endif

    log("Giving 0.5s for Apps to initialize");
    delay(500);

    apps->setActive(0);
    applyConfig(apps->getActiveMotorConfig(), false);
    motor_task_.addListener(knob_state_queue_);

    plaintext_protocol_.init([this]()
                             { changeConfig(0); },
                             [this]()
                             {
                                 if (!configuration_loaded_)
                                 {
                                     return;
                                 }
                                 if (strain_calibration_step_ == 0)
                                 {
                                     log("Strain calibration step 1: Don't touch the knob, then press 'S' again");
                                     strain_calibration_step_ = 1;
                                 }
                                 else if (strain_calibration_step_ == 1)
                                 {
                                     configuration_value_.strain.idle_value = strain_reading_;
                                     snprintf(buf_, sizeof(buf_), "  idle_value=%d", configuration_value_.strain.idle_value);
                                     log(buf_);
                                     log("Strain calibration step 2: Push and hold down the knob with medium pressure, and press 'S' again");
                                     strain_calibration_step_ = 2;
                                 }
                                 else if (strain_calibration_step_ == 2)
                                 {
                                     configuration_value_.strain.press_delta = strain_reading_ - configuration_value_.strain.idle_value;
                                     configuration_value_.has_strain = true;
                                     snprintf(buf_, sizeof(buf_), "  press_delta=%d", configuration_value_.strain.press_delta);
                                     log(buf_);
                                     log("Strain calibration complete! Saving...");
                                     strain_calibration_step_ = 0;
                                     if (configuration_->setStrainCalibrationAndSave(configuration_value_.strain))
                                     {
                                         log("  Saved!");
                                     }
                                     else
                                     {
                                         log("  FAILED to save config!!!");
                                     }
                                 }
                             });

    // Start in legacy protocol mode
    current_protocol_ = &plaintext_protocol_;

    ProtocolChangeCallback protocol_change_callback = [this](uint8_t protocol)
    {
        switch (protocol)
        {
        case SERIAL_PROTOCOL_LEGACY:
            current_protocol_ = &plaintext_protocol_;
            break;
        case SERIAL_PROTOCOL_PROTO:
            current_protocol_ = &proto_protocol_;
            break;
        default:
            log("Unknown protocol requested");
            break;
        }
    };

    plaintext_protocol_.setProtocolChangeCallback(protocol_change_callback);
    proto_protocol_.setProtocolChangeCallback(protocol_change_callback);

    // Interface loop:
    while (1)
    {

        if (xQueueReceive(connectivity_status_queue_, &latest_connectivity_state_, 0) == pdTRUE)
        {
            // do nothing yet
            // log("new connectivity state recieved");
        }

        if (xQueueReceive(knob_state_queue_, &latest_state_, 0) == pdTRUE)
        {

            // combine to the AppState for the display
            AppState app_state = {
                latest_state_,
                latest_connectivity_state_,
            };

            // log(app_state.connectivity_state.ssid.c_str());

            apps->update(app_state); // -> requested change event
            publish(app_state);
            publishState();
        }

        current_protocol_->loop();

        std::string *log_string;
        while (xQueueReceive(log_queue_, &log_string, 0) == pdTRUE)
        {
            current_protocol_->log(log_string->c_str());
            delete log_string;
        }

        updateHardware();

        if (!configuration_loaded_)
        {
            SemaphoreGuard lock(mutex_);
            if (configuration_ != nullptr)
            {
                configuration_value_ = configuration_->get();
                configuration_loaded_ = true;
            }
        }

        delay(1);
    }
}

void AppTask::log(const char *msg)
{
    // Allocate a string for the duration it's in the queue; it is free'd by the queue consumer
    std::string *msg_str = new std::string(msg);

    // Put string in queue (or drop if full to avoid blocking)
    xQueueSendToBack(log_queue_, &msg_str, 0);
}

void AppTask::changeConfig(uint32_t id)
{
    apps->setActive(id);

    applyConfig(apps->getActiveMotorConfig(), false);
}

void AppTask::updateHardware()
{
    // How far button is pressed, in range [0, 1]
    float press_value_unit = 0;

#if SK_ALS
    const float LUX_ALPHA = 0.005;
    static float lux_avg;
    float lux = veml.readLux();
    lux_avg = lux * LUX_ALPHA + lux_avg * (1 - LUX_ALPHA);
    static uint32_t last_als;
    if (millis() - last_als > 1000 && strain_calibration_step_ == 0)
    {
        snprintf(buf_, sizeof(buf_), "millilux: %.2f", lux * 1000);
        log(buf_);
        last_als = millis();
    }
#endif

    static bool pressed;
#if SK_STRAIN
    if (scale.wait_ready_timeout(100))
    {
        strain_reading_ = scale.read();

        static uint32_t last_reading_display;
        if (millis() - last_reading_display > 1000 && strain_calibration_step_ == 0)
        {
            snprintf(buf_, sizeof(buf_), "HX711 reading: %d", strain_reading_);
            log(buf_);
            last_reading_display = millis();
        }
        if (configuration_loaded_ && configuration_value_.has_strain && strain_calibration_step_ == 0)
        {
            // TODO: calibrate and track (long term moving average) idle point (lower)
            press_value_unit = lerp(strain_reading_, configuration_value_.strain.idle_value, configuration_value_.strain.idle_value + configuration_value_.strain.press_delta, 0, 1);

            // Ignore readings that are way out of expected bounds
            if (-1 < press_value_unit && press_value_unit < 2)
            {
                static uint8_t press_readings;
                if (!pressed && press_value_unit > 1)
                {
                    press_readings++;
                    if (press_readings > 2)
                    {
                        motor_task_.playHaptic(true);
                        pressed = true;
                        press_count_++;
                        publishState();
                        if (!remote_controlled_)
                        {
                            changeConfig(apps->navigationNext());
                        }
                    }
                }
                else if (pressed && press_value_unit < 0.5)
                {
                    press_readings++;
                    if (press_readings > 2)
                    {
                        motor_task_.playHaptic(false);
                        pressed = false;
                    }
                }
                else
                {
                    press_readings = 0;
                }
            }
        }
    }
    else
    {
        log("HX711 not found.");

#if SK_LEDS
        for (uint8_t i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = CRGB::Red;
        }
        FastLED.show();
#endif
    }
#endif

    uint16_t brightness = UINT16_MAX;
// TODO: brightness scale factor should be configurable (depends on reflectivity of surface)
#if SK_ALS
    brightness = (uint16_t)CLAMP(lux_avg * 13000, (float)1280, (float)UINT16_MAX);
#endif

#if SK_DISPLAY
    display_task_->setBrightness(brightness); // TODO: apply gamma correction
#endif

#if SK_LEDS
    for (uint8_t i = 0; i < NUM_LEDS; i++)
    {
        leds[i].setHSV(latest_config_.led_hue, 255 - 180 * CLAMP(press_value_unit, (float)0, (float)1) - 75 * pressed, brightness >> 8);

        // Gamma adjustment
        leds[i].r = dim8_video(leds[i].r);
        leds[i].g = dim8_video(leds[i].g);
        leds[i].b = dim8_video(leds[i].b);
    }
    FastLED.show();
#endif
}

void AppTask::setConfiguration(Configuration *configuration)
{
    SemaphoreGuard lock(mutex_);
    configuration_ = configuration;
}

QueueHandle_t AppTask::getConnectivityStateQueue()
{
    return connectivity_status_queue_;
}

void AppTask::addListener(QueueHandle_t queue)
{
    listeners_.push_back(queue);
}

void AppTask::publish(const AppState &state)
{
    for (auto listener : listeners_)
    {
        xQueueOverwrite(listener, &state);
    }
}

void AppTask::publishState()
{
    // Apply local state before publishing to serial
    latest_state_.press_nonce = press_count_;
    current_protocol_->handleState(latest_state_);
}

void AppTask::applyConfig(PB_SmartKnobConfig config, bool from_remote)
{
    remote_controlled_ = from_remote;
    latest_config_ = config;
    motor_task_.setConfig(config);
}
