#pragma once

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include <esp_event.h>
#include <esp_log.h>
#include <esp_http_client.h>

class HTTPRequest
{
    static constexpr char    Tag[] = "http-request";
    EventGroupHandle_t       eventGroup;
    char*                    buffer      = nullptr;
    uint32_t                 bufferLen   = 0;
    uint32_t                 receivedLen = 0;
    char*                    url         = nullptr;
    esp_http_client_method_t method;
    static esp_err_t         httpEventHandler(esp_http_client_event_t* evt)
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
        case HTTP_EVENT_ON_FINISH: {
            ESP_LOGI("OpenWeatherAPI", "Received data (%lu): %s", request->receivedLen,
                     request->buffer);

            xEventGroupSetBits(request->eventGroup, 0x01); // TODO:

            break;
        }
        default:
            break;
        }
        return ESP_OK;
    }

public:
    HTTPRequest(char* _url, esp_http_client_method_t _method, char* _buffer, uint32_t _bufferLen)
        : buffer{ _buffer }, bufferLen{ _bufferLen }, url{ _url }, method{ _method }
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
            return false;

        esp_err_t                retVal = ESP_OK;
        esp_http_client_config_t config = {
            .url = url, .method = method, .event_handler = httpEventHandler, .user_data = this
        };
        receivedLen = 0;

        ESP_LOGI(Tag, "starting https request - %s", config.url);

        esp_http_client_handle_t client = esp_http_client_init(&config);
        esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");
        retVal = esp_http_client_perform(client);

        if ((retVal != ESP_OK) ||
            ((xEventGroupWaitBits(eventGroup, 0x01, pdFALSE, pdFALSE, portMAX_DELAY) & 0x01) ==
             false))
            ESP_LOGE(Tag, "esp_http_client_perform err(%d)", retVal);
        else
            retVal = receivedLen;

        esp_http_client_cleanup(client);
        return retVal;
    }
};