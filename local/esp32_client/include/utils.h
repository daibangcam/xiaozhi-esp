#pragma once
#include "setup.h"
#include <driver/i2s.h>

void clear_queue_and_free(QueueHandle_t queue);
String urlencode(const String &str);
// void sleep_config();
String get_value_after_colon(const String &msg);