#if SK_DISPLAY
#include "display_task.h"
#include "semaphore_guard.h"
#include "util.h"

#include "cJSON.h"
// #include "networking_task.h"

#include "font/roboto_light_60.h"
#include "font/roboto_thin_bold_24.h"
#include "font/roboto_thin_20.h"

static const uint8_t LEDC_CHANNEL_LCD_BACKLIGHT = 0;

DisplayTask::DisplayTask(const uint8_t task_core) : Task{"Display", 2048 * 4, 1, task_core}
{
  app_state_queue_ = xQueueCreate(1, sizeof(AppState));
  assert(app_state_queue_ != NULL);

  mutex_ = xSemaphoreCreateMutex();
  assert(mutex_ != NULL);
}

DisplayTask::~DisplayTask()
{
  vQueueDelete(app_state_queue_);
  vSemaphoreDelete(mutex_);
}

static void drawPlayButton(TFT_eSprite &spr, int x, int y, int width, int height, uint16_t color)
{
  spr.fillTriangle(
      x, y - height / 2,
      x, y + height / 2,
      x + width, y,
      color);
}

Apps *DisplayTask::getApps()
{
  return &apps;
}

void DisplayTask::run()
{
  tft_.begin();
  tft_.invertDisplay(1);
  tft_.setRotation(SK_DISPLAY_ROTATION);
  tft_.fillScreen(TFT_DARKGREEN);

  ledcSetup(LEDC_CHANNEL_LCD_BACKLIGHT, 5000, SK_BACKLIGHT_BIT_DEPTH);
  ledcAttachPin(PIN_LCD_BACKLIGHT, LEDC_CHANNEL_LCD_BACKLIGHT);
  ledcWrite(LEDC_CHANNEL_LCD_BACKLIGHT, (1 << SK_BACKLIGHT_BIT_DEPTH) - 1);

  log("push menu sprite: ok");

  spr_.setColorDepth(8);

  if (spr_.createSprite(TFT_WIDTH, TFT_HEIGHT) == nullptr)
  {
    log("ERROR: sprite allocation failed!");
    tft_.fillScreen(TFT_RED);
  }
  else
  {
    log("Sprite created!");
    tft_.fillScreen(TFT_PURPLE);
  }
  spr_.setTextColor(0xFFFF, TFT_BLACK);

  SettingsApp *settings_app = new SettingsApp(&spr_);

  std::string apps_config = "[{\"app_slug\":\"light_switch\",\"entity_id\":\"light.ceiling\",\"friendly_name\":\"Ceiling\",\"area\":\"Kitchen\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"light_dimmer\",\"entity_id\":\"light.workbench\",\"friendly_name\":\"Workbench\",\"area\":\"Kitchen\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"thermostat\",\"entity_id\":\"climate.office\",\"friendly_name\":\"Climate\",\"area\":\"Office\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"3d_printer\",\"entity_id\":\"3d_printer.office\",\"friendly_name\":\"3D Printer\",\"area\":\"Office\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"blinds\",\"entity_id\":\"blinds.office\",\"friendly_name\":\"Shades\",\"area\":\"Office\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"music\",\"entity_id\":\"music.office\",\"friendly_name\":\"Music\",\"area\":\"Office\",\"menu_color\":\"#ffffff\"}]";

  cJSON *json_root = cJSON_Parse(apps_config.c_str());

  if (json_root == NULL)
  {
    ESP_LOGE("display_task.cpp", "failed to parse JSON");
  }

  apps.setSprite(&spr_);

  cJSON *json_app = NULL;

  uint16_t app_position = 1;

  cJSON_ArrayForEach(json_app, json_root)
  {
    cJSON *json_app_slug = cJSON_GetObjectItemCaseSensitive(json_app, "app_slug");
    cJSON *json_entity_id = cJSON_GetObjectItemCaseSensitive(json_app, "entity_id");
    cJSON *json_friendly_name = cJSON_GetObjectItemCaseSensitive(json_app, "friendly_name");
    snprintf(buf_, sizeof(buf_), "fromJSON > app_slug=%s", json_app_slug->valuestring);
    log(buf_);
    // ESP_LOGD("display_task.cpp", "%s", buf_);

    apps.loadApp(app_position, std::string(json_app_slug->valuestring), std::string(json_entity_id->valuestring), json_friendly_name->valuestring);

    app_position++;
  }

  apps.add(app_position, settings_app);

  // generate menu from apps list
  apps.updateMenu();

  AppState app_state;

  spr_.setTextDatum(CC_DATUM);
  spr_.setTextColor(TFT_WHITE);

  while (1)
  {
    if (xQueueReceive(app_state_queue_, &app_state, portMAX_DELAY) == pdFALSE)
    {
      continue;
    }

    spr_.fillSprite(TFT_BLACK);
    apps.renderActive()->pushSprite(0, 0);

    {
      SemaphoreGuard lock(mutex_);
      ledcWrite(LEDC_CHANNEL_LCD_BACKLIGHT, brightness_);
    }
    delay(5);
  }
}

QueueHandle_t DisplayTask::getKnobStateQueue()
{
  return app_state_queue_;
}

void DisplayTask::setBrightness(uint16_t brightness)
{
  SemaphoreGuard lock(mutex_);
  brightness_ = brightness >> (16 - SK_BACKLIGHT_BIT_DEPTH);
}

void DisplayTask::setLogger(Logger *logger)
{
  logger_ = logger;
}

void DisplayTask::log(const char *msg)
{
  if (logger_ != nullptr)
  {
    logger_->log(msg);
  }
}

#endif