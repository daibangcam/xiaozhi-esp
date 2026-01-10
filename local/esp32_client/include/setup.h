#pragma once
#include <Arduino.h>
#include <freertos/queue.h>
#include <esp_heap_caps.h>
#include <atomic>
#include <esp_system.h>
#include <cstdlib>
#include <ctime>
#include <vector>

struct PcmChunk
{
    int16_t *pcm;
    size_t bytes;
};

struct AnimationInfo
{
    const uint8_t *const *frames;
    const uint16_t *frames_size;
    uint16_t num_frames;
};

struct ImageInfo
{
    const uint8_t *const image;
    const uint16_t image_size;
};

enum State
{
    LOADING,
    LISTENING,
    THINKING,
    SPEAKING,
    PLAY_MUSIC,
    DISCONNECTED,
    SLEEP,
    WAKE_UP,
};

enum StreamType
{
    CHAT,
    MUSIC
};

enum Activity
{
    KEYBOARD,
    SING,
    READ,
    WATCH,
    RELAX,
    NONE,
};

struct GlobalContext
{
    QueueHandle_t mic_to_server = nullptr;
    QueueHandle_t temp_is_silent = nullptr;
    std::atomic<bool> mic_enabled{false};
    std::atomic<bool> spk_enabled{true};
    std::atomic<bool> pcm_sending{false};
    std::atomic<bool> display_video_enabled{true};
    std::atomic<bool> activated{false};
    std::atomic<uint8_t> volume{10};
    std::atomic<State> state{LOADING};
    std::atomic<Activity> activity{NONE};
    String CLIENT_ID;
};

extern GlobalContext g_context;
extern SemaphoreHandle_t tft_mutex;

// thay đổi các định nghĩa chân theo phần cứng bạn sử dụng
// MIC (INMP441 hoặc tương tự)
#define I2S_MIC_PORT I2S_NUM_0
#define I2S_MIC_SCK 5
#define I2S_MIC_WS 4
#define I2S_MIC_SD 6
#define I2S_MIC_SAMPLE_RATE 16000
#define I2S_MIC_DMA_BUF_LEN 512
#define I2S_MIC_DMA_BUF_COUNT 4
#define READ_INTERVAL 32

// LOA (MAX98357A hoặc tương tự)
#define I2S_SPK_PORT I2S_NUM_1
#define I2S_SPK_BCLK 15
#define I2S_SPK_LRCK 16
#define I2S_SPK_DOUT 7
#define I2S_SPK_SAMPLE_RATE 16000
#define I2S_SPK_DMA_BUF_LEN 512
#define I2S_SPK_DMA_BUF_COUNT 6
#define WRITE_INTERVAL 32

// MÀN HÌNH (GC9A01 – SPI)
// #define TFT_CS 41
// #define TFT_DC 40
// #define TFT_RST 42
// #define TFT_SDA 47
// #define TFT_SCL 21
// #define TFT_BL -1
// #define SPI_FREQUENCY 80000000

// LED
#define LED_PIN 48
#define LED_COUNT 1

// TOUCH
#define TOUCH_PIN 8
