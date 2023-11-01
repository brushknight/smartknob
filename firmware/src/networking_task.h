#pragma once

#if SK_NETWORKING

#include <Arduino.h>
#include <WiFi.h>
#include <vector>

#include "logger.h"
#include "proto_gen/smartknob.pb.h"
#include "task.h"
#include "app_config.h"

class NetworkingTask : public Task<NetworkingTask>
{
    friend class Task<NetworkingTask>; // Allow base Task to invoke protected run()

public:
    NetworkingTask(const uint8_t task_core);
    ~NetworkingTask();

    QueueHandle_t getKnobStateQueue();

    void setLogger(Logger *logger);
    void addStateListener(QueueHandle_t queue);

protected:
    void run();

private:
    QueueHandle_t knob_state_queue_;

    std::vector<QueueHandle_t> state_listeners_;

    PB_SmartKnobState state_;
    SemaphoreHandle_t mutex_;
    Logger *logger_;
    void log(const char *msg);
    WiFiClient wifi_client;
    void setup_wifi();
    void publishState(const ConnectivityState &state);
};

#else

class NetworkingTask
{
};

#endif
