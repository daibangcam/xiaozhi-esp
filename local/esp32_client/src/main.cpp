#include "setup.h"
#include "touch.h"
#include "utils.h"
#include "core/protocol/connection.h"
#include "core/audio/mic.h"
#include "core/display/display.h"
#include "core/display/led.h"
#include "core/audio/spk.h"
#include "core/display/animation.h"

GlobalContext g_context;
Mic *mic_instance = nullptr;
Speaker *spk_instance = nullptr;
Connection *con_instance = nullptr;
Animation *animation_instance = nullptr;
SemaphoreHandle_t tft_mutex = nullptr;

void setup()
{
  Serial.begin(115200);

  mic_instance = new Mic(g_context);
  spk_instance = new Speaker(g_context);
  con_instance = new Connection(g_context);
  animation_instance = new Animation(g_context);
  tft_mutex = xSemaphoreCreateMutex();

  g_context.mic_to_server = xQueueCreate(64, sizeof(PcmChunk));
  g_context.temp_is_silent = xQueueCreate(16, sizeof(PcmChunk));
  uint64_t chipid = ESP.getEfuseMac();
  char macStr[13];
  sprintf(macStr, "%012llx", chipid & 0xFFFFFFFFFFFFULL);
  g_context.CLIENT_ID = String(macStr);
  Object::init(mic_instance, spk_instance, con_instance, animation_instance);

  display_init();
  touch_init();

  animation_instance->animation_init();
  animation_instance->start_animation_task();
  con_instance->wifi_connect();
  con_instance->start_send_pcm_task();
  con_instance->ws_start();
  mic_instance->mic_init();
  mic_instance->start_mic_task();
  spk_instance->spk_init();
}

void loop()
{
  if (con_instance->is_activate_client())
  {
    con_instance->ws_loop();
    spk_instance->audio_loop();
  }

  vTaskDelay(pdMS_TO_TICKS(1));
}