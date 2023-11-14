// co-authored by carlhampuswall

#if SK_NETWORKING
#include "networking_task.h"
#include "semaphore_guard.h"
#include "util.h"
#include "wifi_config.h"

NetworkingTask::NetworkingTask(const uint8_t task_core) : Task{"Networking", 2048 * 2, 1, task_core}
{
    mutex_ = xSemaphoreCreateMutex();

    entity_state_to_send_queue_ = xQueueCreate(50, sizeof(EntityStateUpdate));
    assert(entity_state_to_send_queue_ != NULL);
    assert(mutex_ != NULL);
}

NetworkingTask::~NetworkingTask()
{
    vQueueDelete(entity_state_to_send_queue_);

    vSemaphoreDelete(mutex_);
}

void NetworkingTask::enqueueEntityStateToSend(EntityStateUpdate state)
{
    xQueueSendToBack(entity_state_to_send_queue_, &state, 0);
}

void NetworkingTask::setup_wifi()
{
    const char *wifi_name = WIFI_SSID;
    const char *wifi_pass = WIFI_PASSWORD;

    const char *mqtt_host = MQTT_SERVER;
    const uint16_t mqtt_port = 8040;
    const char *mqtt_user = MQTT_USER;
    const char *mqtt_pass = MQTT_PASSWORD;

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

    log("starting MQTT client");
    mqtt = new Mqtt(mqtt_host, mqtt_port, "smartknob", mqtt_user, mqtt_pass);
    mqtt->publish(mqtt_topic_integration, "{\"message\": \"Hello from SmartKnob\"}");
}

void NetworkingTask::run()
{
    // connect to wifi first

    const uint16_t mqtt_push_interval_ms = 200;

    setup_wifi();
    static uint32_t last_wifi_status;

    static uint32_t mqtt_pull;
    static uint32_t mqtt_push; // to prevent spam

    std::map<std::string, EntityStateUpdate> entity_states_to_send;

    EntityStateUpdate entity_state_to_process_;

    char buf_[128];
    while (1)
    {
        if (millis() - mqtt_pull > 1000)
        {
            mqtt->loop();
            mqtt_pull = millis();
        }

        if (millis() - last_wifi_status > 1000)
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
            last_wifi_status = millis();
        }

        // push to the queue;

        if (xQueueReceive(entity_state_to_send_queue_, &entity_state_to_process_, 0) == pdTRUE)
        {

            if (entity_state_to_process_.changed)
            {
                char buf_[128];
                sprintf(buf_,
                        "%s_%s",
                        entity_state_to_process_.entity_name.c_str(),
                        entity_state_to_process_.app_slug);

                entity_states_to_send[buf_] = entity_state_to_process_;

                // entity_states_to_send.insert(std::make_pair(buf_, entity_state_to_process_));
            }
        }

        if (millis() - mqtt_push > mqtt_push_interval_ms)
        {

            // iterate over all items in the map and push all not pushed yet
            for (auto i : entity_states_to_send)
            {
                if (!entity_states_to_send[i.first].sent)
                {
                    sprintf(buf_,
                            "{\"entity_id\": \"%s\", \"app_slug\": \"%s\", \"new_value\": %.2f}",
                            entity_states_to_send[i.first].entity_name.c_str(),
                            entity_states_to_send[i.first].app_slug,
                            entity_states_to_send[i.first].new_value);

                    mqtt->publish(mqtt_to_hass, buf_);

                    entity_states_to_send[i.first].sent = true;

                    // sprintf(buf_, "%s -> %.2f", entity_state_to_send.entity_name.c_str(), entity_state_to_send.new_value);
                    log(buf_);
                }
            }

            mqtt_push = millis();
        }

        delay(5);
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