#include "utils.h"

void clear_queue_and_free(QueueHandle_t queue)
{
    PcmChunk chunk;
    while (xQueueReceive(queue, &chunk, 0) == pdTRUE)
    {
        if (chunk.pcm)
            heap_caps_free(chunk.pcm);
    }
    xQueueReset(queue);
}

String urlencode(const String &str)
{
    String encoded = "";
    char c;
    for (int i = 0; i < str.length(); i++)
    {
        c = str[i];
        if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~')
            encoded += c;
        else
        {
            encoded += '%';
            char buf[3];
            sprintf(buf, "%02X", (unsigned char)c);
            encoded += buf;
        }
    }
    return encoded;
}

String get_value_after_colon(const String &msg)
{
    int idx = msg.indexOf(':');
    if (idx == -1)
        return "";
    return msg.substring(idx + 1);
}
