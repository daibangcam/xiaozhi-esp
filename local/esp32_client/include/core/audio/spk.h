#pragma once

#include "setup.h"
#include <driver/i2s.h>
#include <Audio.h>
#include <LittleFS.h>
#include "core/audio/mic.h"
#include "core/display/display.h"
#include "utils.h"
#include "touch.h"

class Mic;
class Speaker
{
public:
    Speaker(GlobalContext &context);
    ~Speaker();

    void spk_init();
    void audio_loop();
    void start_music();
    void start_chat();
    void set_volume(uint8_t volume);
    void set_music_name(const String &text);
    void start_beep();
    void set_music_id(String music_id);
    String get_music_name();

private:
    GlobalContext &context;
    Audio *audio;
    String music_name;
    String music_id;
    String stream_url;
    uint8_t volume;
    StreamType stream_type;
    String protocol;
    unsigned long time_detect_stop_music;
    bool chat_enabled;
    bool music_enabled;
    bool beep_enabled;
    bool ready_stream;

    void end_music();
    void setup_stream();
    void setup_spk();
};
