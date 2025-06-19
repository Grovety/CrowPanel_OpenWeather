#pragma once

#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include <esp_event.h>
#include <esp_log.h>
#include <esp_http_client.h>
#include <esp_crt_bundle.h>

class HTTPRequest
{
    static constexpr char Tag[] = "http-request";

    EventGroupHandle_t       eventGroup;
    esp_http_client_config_t config;
    char*                    buffer      = nullptr;
    uint32_t                 bufferLen   = 0;
    uint32_t                 receivedLen = 0;

    const uint8_t* body        = nullptr;
    size_t         bodySize    = 0;
    const char*    contentType = nullptr;
    const char*    authHeader  = nullptr;

    static esp_err_t httpEventHandler(esp_http_client_event_t* evt)
    {
        HTTPRequest* request = reinterpret_cast<HTTPRequest*>(evt->user_data);
        switch (evt->event_id)
        {
        case HTTP_EVENT_ON_DATA:
            if (request->bufferLen < (request->receivedLen + evt->data_len))
            {
                ESP_LOGE(Tag, "Not enough space");
                break;
            }
            memcpy(request->buffer + request->receivedLen, evt->data, evt->data_len);
            request->receivedLen += evt->data_len;
            break;
        case HTTP_EVENT_ON_FINISH:
            xEventGroupSetBits(request->eventGroup, 0x01);
            break;
        default:
            break;
        }
        return ESP_OK;
    }

public:
    class Builder
    {
        esp_http_client_config_t config      = { 0 };
        char*                    buffer      = nullptr;
        uint32_t                 bufferLen   = 0;
        const uint8_t*           body        = nullptr;
        size_t                   bodySize    = 0;
        const char*              contentType = nullptr;
        const char*              authHeader  = nullptr;

    public:
        Builder& setUrl(const char* url)
        {
            config.url = url;
            return *this;
        }

        Builder& setHost(const char* host, int port, const char* path)
        {
            config.host = host;
            config.port = port;
            config.path = path;
            return *this;
        }

        Builder& setMethod(esp_http_client_method_t method)
        {
            config.method = method;
            return *this;
        }

        Builder& setBuffer(char* _buffer, uint32_t _bufferLen)
        {
            buffer    = _buffer;
            bufferLen = _bufferLen;
            return *this;
        }

        Builder& setSSL(esp_err_t (*crtBundleAttach)(void*))
        {
            config.transport_type    = HTTP_TRANSPORT_OVER_SSL;
            config.crt_bundle_attach = crtBundleAttach;
            return *this;
        }

        Builder& setBody(const uint8_t* _body, size_t _bodySize, const char* _contentType)
        {
            body        = _body;
            bodySize    = _bodySize;
            contentType = _contentType;
            return *this;
        }

        Builder& setAuthHeader(const char* _authHeader)
        {
            authHeader = _authHeader;
            return *this;
        }

        HTTPRequest build()
        {
            return HTTPRequest(config, buffer, bufferLen, body, bodySize, contentType, authHeader);
        }
    };

private:
    HTTPRequest(const esp_http_client_config_t& cfg, char* buf, uint32_t bufLen,
                const uint8_t* _body, size_t _bodySize, const char* _contentType,
                const char* _authHeader)
        : config(cfg), buffer(buf), bufferLen(bufLen), body(_body), bodySize(_bodySize),
          contentType(_contentType), authHeader(_authHeader)
    {
        eventGroup           = xEventGroupCreate();
        config.event_handler = httpEventHandler;
        config.user_data     = this;
    }

public:
    ~HTTPRequest()
    {
        vEventGroupDelete(eventGroup);
    }

    esp_err_t perform()
    {
        receivedLen = 0;
        if (buffer && bufferLen > 0)
            buffer[0] = '\0';

        esp_http_client_handle_t client = esp_http_client_init(&config);
        if (!client)
        {
            ESP_LOGE(Tag, "Client init failed");
            return ESP_FAIL;
        }

        if (contentType)
            esp_http_client_set_header(client, "Content-Type", contentType);

        if (authHeader)
            esp_http_client_set_header(client, "Authorization", authHeader);

        if (body && bodySize > 0)
            esp_http_client_set_post_field(client, reinterpret_cast<const char*>(body), bodySize);

        ESP_LOGI(Tag, "Starting request");

        esp_err_t err = esp_http_client_perform(client);

        if (err != ESP_OK ||
            ((xEventGroupWaitBits(eventGroup, 0x01, pdFALSE, pdFALSE, portMAX_DELAY) & 0x01) == 0))
        {
            ESP_LOGE(Tag, "HTTP perform failed: %s", esp_err_to_name(err));
            esp_http_client_cleanup(client);
            return ESP_FAIL;
        }

        esp_http_client_cleanup(client);
        return receivedLen;
    }
};