#include "core/audio/spk.h"

Speaker::Speaker(GlobalContext &context) : context(context)
{
    this->chat_enabled = false;
    this->music_enabled = false;
    this->beep_enabled = false;
    this->ready_stream = false;
    this->time_detect_stop_music = 0;
    this->volume = 10;
    LittleFS.begin();
}
Speaker::~Speaker()
{
    if (this->audio)
    {
        delete this->audio;
        this->audio = nullptr;
    }
}
void Speaker::spk_init()
{

    this->audio = new Audio(false, 3, I2S_NUM_1);
    this->audio->setPinout(I2S_SPK_BCLK, I2S_SPK_LRCK, I2S_SPK_DOUT);
    this->audio->setVolume(this->volume);
    if (Object::con->get_port() == 443)
        this->protocol = "https://";
    else
        this->protocol = "http://";
}

void Speaker::setup_stream()
{
    String type;

    if (this->stream_type == StreamType::CHAT)
        type = "stream_chat?client_id=" + this->context.CLIENT_ID;
    else if (this->stream_type == StreamType::MUSIC)
        type = "stream_music?client_id=" + this->context.CLIENT_ID + "&music_id=" + this->music_id;

    this->stream_url = this->protocol + Object::con->get_host_name() + "/" + type;
    this->ready_stream = true;
}

void Speaker::start_beep()
{
    this->audio->connecttoFS(LittleFS, "/think.mp3");
    this->beep_enabled = true;
    this->audio->setVolume(10);
}

void Speaker::end_music()
{
    String url = this->protocol + Object::con->get_host_name() + "/" + "stop_stream_music?client_id=" + this->context.CLIENT_ID;
    Object::con->stop_music(url);
    this->audio->pauseResume();
}

void Speaker::audio_loop()
{
    if (this->context.spk_enabled.load())
    {
        this->audio->loop();
        if (!this->audio->isRunning())
        {
            if (this->ready_stream)
            {
                if (this->beep_enabled)
                {
                    this->beep_enabled = false;
                    this->audio->setVolume(this->volume);
                }

                if (this->stream_type == StreamType::CHAT)
                    this->chat_enabled = true;
                else
                {
                    if (this->chat_enabled)
                        this->chat_enabled = false;
                    this->music_enabled = true;
                }

                this->ready_stream = false;
                this->audio->connecttohost(this->stream_url.c_str());
            }
            else if (!this->ready_stream && !this->beep_enabled)
            {
                if (this->chat_enabled)
                {
                    Object::mic->setup_record();
                    this->chat_enabled = false;
                }

                else if (this->music_enabled)
                {
                    Object::mic->setup_record();
                    this->music_enabled = false;
                }
            }
        }
        if (this->music_enabled)
        {
            if (detected_stop_music(this->context))
            {
                if (this->time_detect_stop_music == 0)
                    this->time_detect_stop_music = millis();
                if (millis() - this->time_detect_stop_music >= 1000)
                    this->end_music();
            }
            else if (this->time_detect_stop_music != 0)
                this->time_detect_stop_music = 0;
        }
    }
}

void Speaker::start_music()
{
    this->time_detect_stop_music = 0;
    this->setup_spk();
    this->context.state.store(State::PLAY_MUSIC);
    this->stream_type = StreamType::MUSIC;
    this->setup_stream();
}

void Speaker::start_chat()
{
    this->setup_spk();
    this->stream_type = StreamType::CHAT;
    this->setup_stream();
}

void Speaker::setup_spk()
{
    this->ready_stream = false;
    this->context.mic_enabled.store(false);
    this->context.pcm_sending.store(false);
}
void Speaker::set_volume(uint8_t volume)
{
    this->volume = volume;
    this->audio->setVolume(volume);
}

void Speaker::set_music_name(const String &music_name)
{
    this->music_name = music_name;
}

void Speaker::set_music_id(String music_id)
{
    this->music_id = music_id;
}

String Speaker::get_music_name()
{
    return this->music_name;
}
