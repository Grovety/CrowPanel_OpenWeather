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
    char*                    buffer      = nullptr;
    uint32_t                 bufferLen   = 0;
    uint32_t                 receivedLen = 0;
    const char*              url         = nullptr;
    esp_http_client_method_t method;

    // Дополнительные поля:
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
                ESP_LOGE(Tag, "not enough space");
                break;
            }
            memcpy(request->buffer + request->receivedLen, evt->data, evt->data_len);
            request->receivedLen += evt->data_len;
            break;
        case HTTP_EVENT_ON_FINISH:
            // ESP_LOGI(Tag, "Received data (%lu): %s", request->receivedLen, request->buffer);
            xEventGroupSetBits(request->eventGroup, 0x01);
            break;
        default:
            break;
        }
        return ESP_OK;
    }

public:
    HTTPRequest(const char* _url, esp_http_client_method_t _method, char* _buffer,
                uint32_t _bufferLen, const uint8_t* _body = nullptr, size_t _bodySize = 0,
                const char* _contentType = nullptr, const char* _authHeader = nullptr)
        : buffer{ _buffer }, bufferLen{ _bufferLen }, url{ _url }, method{ _method }, body{ _body },
          bodySize{ _bodySize }, contentType{ _contentType }, authHeader{ _authHeader }
    {
        eventGroup = xEventGroupCreate();
    }

    ~HTTPRequest()
    {
        vEventGroupDelete(eventGroup);
    }

    esp_err_t perform()
    {
        if (!url)
            return ESP_FAIL;

        extern const uint8_t wit_root_ca_pem_start[] asm("_binary_root_cert_pem_start");

        esp_http_client_config_t config = { 0 };
        if (strstr(url, "wit.ai"))
        {
            config.host           = "api.wit.ai";
            config.port           = 443;
            config.path           = "/speech?v=20240520";
            config.transport_type = HTTP_TRANSPORT_OVER_SSL;

            // config.cert_pem        = reinterpret_cast<const char*>(wit_root_ca_pem_start);
            config.method            = method;
            config.event_handler     = httpEventHandler;
            config.user_data         = this;
            config.crt_bundle_attach = esp_crt_bundle_attach;
        } else
        {
            config.url           = url;
            config.method        = method;
            config.event_handler = httpEventHandler;
            config.user_data     = this;
        }

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

        ESP_LOGI(Tag, "Starting request to %s", url);

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
