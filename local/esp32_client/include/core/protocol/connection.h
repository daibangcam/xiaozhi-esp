#pragma once

#include "setup.h"
#include <WiFi.h>
#include <WiFiManager.h>
#include <WebSocketsClient.h>
#include "utils.h"
#include "obj.h"
#include <HTTPClient.h>

class Object;

class Connection
{
public:
    Connection(GlobalContext &context);
    ~Connection();

    void wifi_connect();
    void ws_start();
    void ws_loop();
    void start_send_pcm_task();
    void start_send_pcm();
    void start_sleep();
    void stop_music(String url);
    bool is_activate_client();
    uint16_t get_port();
    String get_host_name();
    String get_time();

private:
    GlobalContext &context;
    WebSocketsClient *ws;
    WiFiManager *wifi_manager;
    String host_name;
    String url_ws;
    uint16_t port;
    String url_handshake;
    static Connection *self;

    static void on_ws_event(WStype_t type, uint8_t *payload, size_t length);
    static void send_pcm_task_wrapper(void *param);
    void send_pcm_task();
    void update_state();

    String try_handshake(String url);
};

const char root_ca[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
...
-----END CERTIFICATE-----
)EOF"; // nếu dùng https thì thêm chứng chỉ root ca của server vào đây
