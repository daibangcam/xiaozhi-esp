#include "touch.h"

void touch_init()
{
    pinMode(TOUCH_PIN, INPUT);
    digitalWrite(TOUCH_PIN, LOW);
}

bool detected_stop_music(GlobalContext &context)
{
    int state = digitalRead(TOUCH_PIN);
    if (state == HIGH)
        return true;
    else
        return false;
}