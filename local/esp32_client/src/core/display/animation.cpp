#include "core/display/animation.h"
#include "core/display/display.h"
#include "animation/angry.h"
#include "animation/blink.h"
#include "animation/disconnected.h"
#include "animation/light_blink.h"
#include "animation/light_shake_01.h"
#include "animation/light_shake_02.h"
#include "animation/loading.h"
#include "animation/roll.h"
#include "animation/sad.h"
#include "animation/shake.h"
#include "animation/squint.h"
#include "animation/think.h"
#include "animation/tilt_right.h"
#include "animation/wink.h"
#include "animation/sleep.h"
#include "animation/wake_up.h"

Animation::Animation(GlobalContext &context) : context(context)
{
    std::srand(std::time(nullptr));

    animation_list[0] = &is_angry;
    animation_list[1] = &is_blink;
    animation_list[2] = &is_light_blink;
    animation_list[3] = &is_roll;
    animation_list[4] = &is_sad;
    animation_list[5] = &is_shake;
    animation_list[6] = &is_tilt_right;
    animation_list[7] = &is_wink;

    core_animation_list[0] = &is_loading;
    core_animation_list[1] = &is_disconnected;
    core_animation_list[2] = &is_light_shake_01;
    core_animation_list[3] = &is_light_shake_02;
    core_animation_list[4] = &is_think;
    core_animation_list[5] = &is_squint;
    core_animation_list[6] = &is_sleep;
    core_animation_list[7] = &is_wake_up;

    self = this;
}

Animation::~Animation() {}

void Animation::start_animation_task()
{
    xTaskCreatePinnedToCore(Animation::animation_task_wrapper, "video_task", 1024 * 12, this, 1, NULL, 0);
}
void Animation::animation_task_wrapper(void *param)
{
    Animation *self = static_cast<Animation *>(param);
    self->animation_task();
}

void Animation::animation_state(State current_state)
{
    AnimationInfo *current_animation = this->get_animation();
    if (current_state == State::THINKING && !this->audio_anabled)
    {
        Object::spk->start_beep();
        this->audio_anabled = true;
    }

    for (uint16_t i = 0; i < current_animation->num_frames; i++)
    {
        if (!this->context.display_video_enabled.load())
            break;

        this->drawJPEGFrame(current_animation, i, current_state);

        vTaskDelay(pdMS_TO_TICKS(15));
    }
}
void Animation::animation_activity(Activity current_activity)
{
}

void Animation::animation_task()
{
    State new_state;
    State current_state;
    Activity current_activity;
    Activity new_activity;
    while (true)
    {

        if (!this->context.display_video_enabled.load())
        {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        current_activity = this->context.activity.load();
        current_state = this->context.state.load();

        if (current_activity != Activity::NONE)
        {
            if (current_activity == Activity::WATCH)
            {
            }
            this->context.activity.store(Activity::NONE);
        }
        else
            this->animation_state(current_state);

        new_state = this->context.state.load();
        new_activity = this->context.activity.load();

        if (current_state == State::SLEEP)
            this->context.display_video_enabled.store(false);

        if (current_state == State::WAKE_UP)
            this->context.state.store(State::THINKING);

        if (current_state != new_state)
        {
            this->audio_anabled = false;
            if (current_state == State::DISCONNECTED || current_state == State::LOADING ||
                new_state == State::DISCONNECTED || new_state == State::LOADING)
                display_clear();
        }

        vTaskDelay(pdMS_TO_TICKS(15));
    }
}

AnimationInfo *Animation::get_animation()
{
    switch (this->context.state.load())
    {
    case State::LISTENING:
    {
        std::vector<AnimationInfo *> options = {
            this->core_animation_list[2],
            this->core_animation_list[3], // light_shake

            this->animation_list[2], // light_blink
            this->animation_list[6]  // tilt_right
        };
        this->context.activity.store(Activity::NONE);
        return options[rand() % options.size()];
    }
    case State::THINKING:
    {
        std::vector<AnimationInfo *> options = {
            this->core_animation_list[4], // think
            this->animation_list[1],      // blink
            // this->animation_list[3],      // roll
            // this->animation_list[6] // tilt_right
        };
        return options[rand() % options.size()];
    }
    case State::SPEAKING:
    {
        std::vector<AnimationInfo *> options = {
            this->core_animation_list[5], // squint
            this->animation_list[3],      // roll
            this->animation_list[7]       // wink
        };
        return options[rand() % options.size()];
    }
    case State::PLAY_MUSIC:
    {
        std::vector<AnimationInfo *> options = {
            this->core_animation_list[5], // squint
            this->animation_list[3],      // roll
            this->animation_list[7]       // wink
        };

        this->context.activity.store(Activity::SING);
        return options[rand() % options.size()];
    }
    case State::DISCONNECTED:
        return this->core_animation_list[1]; //
    case State::LOADING:
        return this->core_animation_list[0]; //
    case State::SLEEP:
        return this->core_animation_list[6]; //
    case State::WAKE_UP:
        return this->core_animation_list[7]; //
    default:
        return this->core_animation_list[0]; //
    }
}

ImageInfo *Animation::get_image()
{
    return nullptr;
}

void Animation::animation_init()
{
    TJpgDec.setJpgScale(1);
    TJpgDec.setSwapBytes(true);
    TJpgDec.setCallback(this->tft_output_wrapper);
}

inline bool Animation::is_near_black(uint16_t px)
{
    uint8_t r = (px >> 11) & 0x1F;
    uint8_t g = (px >> 5) & 0x3F;
    uint8_t b = px & 0x1F;

    r = r << 3;
    g = g << 2;
    b = b << 3;

    return (r < 40 && g < 40 && b < 40);
}

bool Animation::tft_output_wrapper(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bit_map)
{
    if (!tft_mutex)
        return false;

    if (x >= tft.width() || y >= tft.height())
        return false;

    if (xSemaphoreTake(tft_mutex, portMAX_DELAY) == pdTRUE)
    {
        tft.pushImage(x, y, w, h, bit_map);
        xSemaphoreGive(tft_mutex);
    }
    return true;
}

void Animation::drawJPEGFrame(const AnimationInfo *video, uint16_t frame_index, State current_state)
{
    const uint8_t *jpg_data = (const uint8_t *)pgm_read_ptr(&video->frames[frame_index]);
    uint16_t jpg_size = pgm_read_word(&video->frames_size[frame_index]);
    if (current_state == State::DISCONNECTED)
        TJpgDec.drawJpg(80, 95, jpg_data, jpg_size);
    else if (current_state == State::LOADING)
        TJpgDec.drawJpg(85, 85, jpg_data, jpg_size);
    else
    {
        TJpgDec.drawJpg(20, 65, jpg_data, jpg_size);
    }
}
void Animation::drawJPEGImage(const ImageInfo *img)
{

    const uint8_t *jpg_data = (const uint8_t *)pgm_read_ptr(&img->image);
    uint16_t jpg_size = img->image_size;
    TJpgDec.drawJpg(40, 100, jpg_data, jpg_size);
}

Animation *Animation::self = nullptr;
