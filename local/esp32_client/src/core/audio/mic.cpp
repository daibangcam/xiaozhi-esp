#include "core/audio/mic.h"
Mic::Mic(GlobalContext &context) : context(context)
{
    this->silence_threshold = 300;
}

Mic::~Mic() {}

void Mic::mic_init()
{
    i2s_config_t cfg = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = I2S_MIC_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = 0,
        .dma_buf_count = I2S_MIC_DMA_BUF_COUNT,
        .dma_buf_len = I2S_MIC_DMA_BUF_LEN,
        .use_apll = true,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0};

    i2s_pin_config_t pin = {
        .bck_io_num = I2S_MIC_SCK,
        .ws_io_num = I2S_MIC_WS,
        .data_out_num = -1,
        .data_in_num = I2S_MIC_SD};

    i2s_driver_install(I2S_MIC_PORT, &cfg, 0, NULL);
    i2s_set_pin(I2S_MIC_PORT, &pin);
}

void Mic::mic_task()
{
    uint8_t buff_size = 3;
    int32_t i2s_read_buff[I2S_MIC_DMA_BUF_LEN * buff_size];
    int16_t buf[I2S_MIC_DMA_BUF_LEN * buff_size];
    size_t bytes_read;
    bool pcm_send;
    int32_t *read_buf = nullptr;
    int16_t *conv_buf = nullptr;
    size_t size;
    PcmChunk chunk;
    while (true)
    {
        if (!this->context.mic_enabled.load())
        {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        read_buf = i2s_read_buff;
        conv_buf = buf;
        size = sizeof(i2s_read_buff);

        i2s_read(I2S_MIC_PORT, read_buf, size, &bytes_read, portMAX_DELAY);
        shift_bit(read_buf, conv_buf, bytes_read / 4);
        int16_t *pcm = (int16_t *)heap_caps_malloc(bytes_read / 2, MALLOC_CAP_SPIRAM);

        if (pcm)
            memcpy(pcm, conv_buf, bytes_read / 2);

        chunk = {pcm, bytes_read / 2};

        if (!is_silent(conv_buf, bytes_read / 4, this->silence_threshold))
        {
            if (uxQueueMessagesWaiting(this->context.temp_is_silent) != 0)
            {
                PcmChunk temp_chunk;
                while (xQueueReceive(this->context.temp_is_silent, &temp_chunk, 0) == pdTRUE)
                    xQueueSend(this->context.mic_to_server, &temp_chunk, 0);
            }
            xQueueSend(this->context.mic_to_server, &chunk, 0);
            UBaseType_t count = uxQueueMessagesWaiting(this->context.mic_to_server);
            update_state(count);
        }
        else
        {
            pcm_send = this->context.pcm_sending.load();

            if (pcm_send)
                xQueueSend(this->context.temp_is_silent, &chunk, 0);
            if (!pcm_send)
            {
                heap_caps_free(pcm);
                this->detect_sleep();
            }
        }
    }
}

void Mic::shift_bit(int32_t *i2s_read_buff, int16_t *buf, size_t len)
{
    for (int i = 0; i < len; i++)
        buf[i] = i2s_read_buff[i] >> 14;
}

bool Mic::is_silent(int16_t *buf, size_t len, uint16_t threshold)
{
    uint64_t sum = 0;
    for (int i = 0; i < len; i++)
        sum += abs(buf[i]);
    return (sum / len) < threshold;
}

void Mic::update_state(UBaseType_t count)
{
    if (count >= 6 && !this->context.pcm_sending.load())
    {
        Object::con->start_send_pcm();
        this->context.pcm_sending.store(true);
    }
}

void Mic::mic_task_wrapper(void *param)
{
    Mic *self = static_cast<Mic *>(param);
    self->mic_task();
}

void Mic::start_mic_task()
{
    xTaskCreatePinnedToCore(mic_task_wrapper, "mic_task", 1024 * 16, this, 3, NULL, 1);
}

void Mic::setup_record()
{
    if (this->context.state.load() != State::SLEEP)
        this->context.state.store(State::LISTENING);
    clear_queue_and_free(this->context.mic_to_server);
    clear_queue_and_free(this->context.temp_is_silent);
    i2s_zero_dma_buffer(I2S_MIC_PORT);
    i2s_start(I2S_MIC_PORT);
    this->context.mic_enabled.store(true);
    this->time_detect_sleep = millis();
}

void Mic::start_sleep()
{
    this->silence_threshold = 500;
    this->context.state.store(State::SLEEP);
    Object::con->start_sleep();
}

void Mic::detect_sleep()
{
    if (millis() - this->time_detect_sleep >= 30000UL && this->context.state.load() != State::SLEEP)
        this->start_sleep();
}

void Mic::set_silence_threshold(uint16_t silence_threshold)
{
    this->silence_threshold = silence_threshold;
}

void Mic::stop_record()
{
    this->context.pcm_sending.store(false);
    this->context.mic_enabled.store(false);
    i2s_stop(I2S_MIC_PORT);
    if (this->context.state.load() != State::SLEEP)
    {
        this->context.state.store(State::THINKING);
        // Object::spk->start_beep();
    }
}
