#if SK_NETWORKING
#include "networking_task.h"
#include "semaphore_guard.h"
#include "util.h"
#include "wifi_config.h"

NetworkingTask::NetworkingTask(const uint8_t task_core) : Task{"Networking", 2048 * 2, 1, task_core}
{
    mutex_ = xSemaphoreCreateMutex();
    assert(mutex_ != NULL);
}

NetworkingTask::~NetworkingTask()
{
    vSemaphoreDelete(mutex_);
}

void NetworkingTask::setup_wifi()
{
    const char *wifi_name = WIFI_SSID;
    const char *wifi_pass = WIFI_PASSWORD;

    WiFi.setHostname("SmartKnob");
    WiFi.setAutoReconnect(true);

    WiFi.begin(wifi_name, wifi_pass);

    while (WiFi.status() != WL_CONNECTED)
    {
        log("connecting to WiFi, waiting");
        delay(500);
    }
    char buf_[128];
    sprintf(buf_, "YooHoo, WiFi connected, ip: %s", WiFi.localIP().toString());
    // TODO: send this state to the UI
    log(buf_);
}

void NetworkingTask::run()
{
    // connect to wifi first

    setup_wifi();

    while (1)
    {

        int8_t rssi = WiFi.RSSI();
        uint8_t signal_strenth_status;

        if (rssi < -110)
        {
            signal_strenth_status = 4;
        }
        else if (rssi < -100)
        {
            signal_strenth_status = 3;
        }
        else if (rssi < -85)
        {
            signal_strenth_status = 2;
        }
        else if (rssi < -70)
        {
            signal_strenth_status = 1;
        }
        else
        {
            signal_strenth_status = 0;
        }

        // harvest state;
        ConnectivityState state = {
            WiFi.isConnected(),
            WiFi.RSSI(),
            signal_strenth_status,
            WIFI_SSID,
            WiFi.localIP().toString().c_str()};

        publishState(state);

        // push to the queue;

        delay(1000);
    }
}

void NetworkingTask::addStateListener(QueueHandle_t queue)
{
    state_listeners_.push_back(queue);
}

void NetworkingTask::publishState(const ConnectivityState &state)
{
    for (auto listener : state_listeners_)
    {
        xQueueOverwrite(listener, &state);
    }
}

void NetworkingTask::setLogger(Logger *logger)
{
    logger_ = logger;
}

void NetworkingTask::log(const char *msg)
{
    if (logger_ != nullptr)
    {
        logger_->log(msg);
    }
}

#endif