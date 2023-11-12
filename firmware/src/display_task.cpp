#if SK_DISPLAY
#include "display_task.h"
#include "semaphore_guard.h"
#include "util.h"

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

  MenuApp *menu_app = new MenuApp(&spr_);
  MusicApp *music_app = new MusicApp(&spr_, "offise_sonos");
  ClimateApp *climate_app = new ClimateApp(&spr_, "kitchen_climate");
  BlindsApp *blinds_app = new BlindsApp(&spr_, "office_blinds");
  SettingsApp *settings_app = new SettingsApp(&spr_);
  LightDimmerApp *light_dimmer_app = new LightDimmerApp(&spr_, "office_ceiling");
  LightSwitchApp *light_switch_app = new LightSwitchApp(&spr_, "office_table");
  PrinterChamberApp *printer_chamber_app = new PrinterChamberApp(&spr_, "3d_printer");

  apps.add(0, menu_app);
  apps.add(1, printer_chamber_app);
  apps.add(2, climate_app);
  apps.add(3, blinds_app);
  apps.add(4, light_switch_app);
  apps.add(5, light_dimmer_app);
  apps.add(6, music_app);
  apps.add(7, settings_app);

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