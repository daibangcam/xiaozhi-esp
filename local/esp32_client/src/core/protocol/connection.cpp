#include "core/protocol/connection.h"

Connection::Connection(GlobalContext &context) : context(context)
{
    self = this;
    this->ws = new WebSocketsClient();
    this->wifi_manager = new WiFiManager();
    this->url_ws = "/chat";
    this->host_name = "192.168.1.?"; // thay bằng ip máy tính chưa server mà bạn muốn kết nối đến
    this->port = 80;
}

Connection::~Connection()
{
    if (this->ws)
    {
        delete this->ws;
        this->ws = nullptr;
    }
    if (this->wifi_manager)
    {
        delete this->wifi_manager;
        this->wifi_manager = nullptr;
    }
}
void Connection::wifi_connect()
{
    this->context.state.store(LOADING);
    bool connected = this->wifi_manager->autoConnect("chatbot_client", "12345678"); // chỉnh tên và mật khẩu wifi của bạn neu cần
    if (connected)
    {
        String url = "http://" + this->host_name + "/handshake?client_id=" + this->context.CLIENT_ID;
        String response = Connection::try_handshake(url);
        if (response == "not_connection")
        {
            // this->host_name = "";
            // this->port = 443;
            // url = "https://" + this->host_name + "/handshake?client_id=" + this->context.CLIENT_ID;
            response = Connection::try_handshake(url);
        }
        response.trim();
        this->url_handshake = url;
        if (response == "false")
            this->context.activated.store(false);
        else
            this->context.activated.store(true);
    }
    else
    {
    }
    configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");
}

void Connection::stop_music(String url)
{
    HTTPClient http;

    http.begin(url);
    int http_code = http.GET();
    http.end();
}

String Connection::try_handshake(String url)
{
    HTTPClient http;

    http.begin(url);
    int http_code = http.GET();
    String response;

    if (http_code > 0)
    {
        if (http_code == 200)
            response = http.getString();
    }
    else
        response = "not_connection";
    http.end();
    return response;
}

void Connection::start_send_pcm()
{
    this->ws->sendTXT("START_CHAT");
}

void Connection::start_sleep()
{
    this->ws->sendTXT("START_SLEEP");
}

void Connection::ws_start()
{
    String end_point = this->url_ws + "?client_id=" + this->context.CLIENT_ID;
    if (this->port == 443)
        this->ws->beginSslWithCA(this->host_name.c_str(), this->port, end_point.c_str(), root_ca); // this->ws->beginSSL(this->host_name, this->port, end_point);
    else
        this->ws->begin(this->host_name, this->port, end_point);
    this->ws->onEvent(on_ws_event);
    this->ws->setReconnectInterval(5000);
    this->ws->enableHeartbeat(15000, 3000, 2);
}

void Connection::on_ws_event(WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_CONNECTED:
    {
        Object::mic->setup_record();
        break;
    }
    case WStype_DISCONNECTED:
    {
        self->context.state.store(DISCONNECTED);
        break;
    }
    case WStype_TEXT:
    {
        String msg = String((char *)payload);

        if (msg.startsWith("READY_CHAT"))
            Object::spk->start_chat();
        if (msg.startsWith("START_STREAM_CHAT"))
        {
            if (self->context.state.load() != State::SLEEP)
                self->context.state.store(State::SPEAKING);
        }
        else if (msg.startsWith("READY_MUSIC"))
        {
            const String music_id = get_value_after_colon(msg);
            Object::spk->set_music_id(music_id);
            Object::spk->start_music();
        }
        else if (msg.startsWith("VOLUME"))
        {
            const String volume = get_value_after_colon(msg);
            Object::spk->set_volume(volume.toInt());
        }

        else if (msg.startsWith("MUSIC_NAME"))
        {
            const String music_name = get_value_after_colon(msg);
            Object::spk->set_music_name(music_name);
        }

        else if (msg.startsWith("START_SLEEP"))

            Object::mic->start_sleep();

        else if (msg.startsWith("END_SLEEP"))
        {
            Object::mic->set_silence_threshold(300);
            self->context.state.store(State::WAKE_UP);
            self->context.display_video_enabled.store(true);
        }

        else if (msg.startsWith("END_CHAT"))
        {
        }

        else if (msg.startsWith("END_MUSIC"))
        {
        }

        else if (msg.startsWith("READ"))
            self->context.activity.store(Activity::READ);

        else if (msg.startsWith("KEYBOARD"))
            self->context.activity.store(Activity::KEYBOARD);

        else if (msg.startsWith("SING"))
            self->context.activity.store(Activity::SING);

        else if (msg.startsWith("WATCH"))
            self->context.activity.store(Activity::WATCH);

        break;
    }
    default:
        break;
    }
}

void Connection::ws_loop()
{
    this->ws->loop();
}

void Connection::start_send_pcm_task()
{
    xTaskCreatePinnedToCore(Connection::send_pcm_task_wrapper, "send_pcm_task", 1024 * 8, this, 2, NULL, 1);
}

void Connection::send_pcm_task_wrapper(void *param)
{
    Connection *self = static_cast<Connection *>(param);
    self->send_pcm_task();
}

void Connection::send_pcm_task()
{
    while (true)
    {
        if (!this->context.pcm_sending.load())
        {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        PcmChunk chunk;
        if (xQueueReceive(this->context.mic_to_server, &chunk, pdMS_TO_TICKS(500)) == pdTRUE)
        {
            if (chunk.pcm && chunk.bytes > 0 && ws->isConnected())
                this->ws->sendBIN((uint8_t *)chunk.pcm, chunk.bytes);
            vTaskDelay(pdMS_TO_TICKS(2));
            heap_caps_free(chunk.pcm);
        }
        else
            update_state();
    }
}

void Connection::update_state()
{
    this->ws->sendTXT("END_CHAT");
    Object::mic->stop_record();
}

uint16_t Connection::get_port()
{
    return this->port;
}

String Connection::get_host_name()
{
    return this->host_name;
}

bool Connection::is_activate_client()
{
    if (this->context.activated.load())
        return true;
    else
    {
        display_clear();
        display_text_center_line("Not activated", 7, 2, TFT_RED);
        display_text_center_line("ID: " + this->context.CLIENT_ID, 9, 2, TFT_WHITE);
        vTaskDelay(pdMS_TO_TICKS(3000));
        String response = Connection::try_handshake(this->url_handshake);
        response.trim();
        if (response == "false")
        {
            this->context.display_video_enabled.store(false);
            this->context.activated.store(false);
            return false;
        }
        else
        {
            this->context.display_video_enabled.store(true);
            this->context.activated.store(true);
            return true;
        }
    }
}

String Connection::get_time()
{
    struct tm timeinfo;

    if (!getLocalTime(&timeinfo))
    {
        return "00:00";
    }

    char buffer[6];
    sprintf(buffer, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);

    return String(buffer);
}

Connection *Connection::self = nullptr;