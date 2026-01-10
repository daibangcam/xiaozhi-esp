#pragma once
#include "setup.h"
#include <driver/i2s.h>
#include "core/protocol/connection.h"
#include "utils.h"
class Connection;
class Mic
{
public:
    Mic(GlobalContext &context);
    ~Mic();
    void mic_init();
    void start_mic_task();
    void setup_record();
    void set_silence_threshold(uint16_t silence_threshold);
    void start_sleep();
    void stop_record();

private:
    GlobalContext &context;
    int64_t time_detect_sleep;
    uint16_t silence_threshold;

    bool is_silent(int16_t *buf, size_t len, uint16_t threshold);
    void shift_bit(int32_t *i2s_read_buff, int16_t *buf, size_t len);
    void update_state(UBaseType_t count);
    void mic_task();
    void detect_sleep();
    static void mic_task_wrapper(void *param);
};
