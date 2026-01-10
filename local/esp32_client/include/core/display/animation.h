#pragma once

#include "setup.h"
#include "core/display/display.h"
#include <TJpg_Decoder.h>
#include "obj.h"
class Animation
{
public:
    Animation(GlobalContext &context);
    ~Animation();
    void start_animation_task();
    void animation_init();

private:
    GlobalContext &context;
    AnimationInfo *animation_list[8];
    AnimationInfo *core_animation_list[8];
    AnimationInfo *get_animation();
    ImageInfo *get_image();
    bool audio_anabled;
    static Animation *self;

    static void animation_task_wrapper(void *param);
    void animation_task();
    void animation_state(State current_state);
    void animation_activity(Activity current_activity);
    inline bool is_near_black(uint16_t px);
    void drawJPEGImage(const ImageInfo *img);
    void drawJPEGFrame(const AnimationInfo *animation, uint16_t frame_index, State current_state);
    static bool tft_output_wrapper(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bit_map);
};
