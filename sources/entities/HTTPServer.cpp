#include "HttpServer.h"
#include <esp_log.h>

// Статическая переменная для доступа из хендлера
WavRecorder* HttpServer::sharedRecorder = nullptr;

HttpServer::HttpServer(WavRecorder& recorder)
{
    sharedRecorder = &recorder;
}

HttpServer::~HttpServer()
{
    stop();
}

esp_err_t HttpServer::handleWav(httpd_req_t* req)
{
    if (!sharedRecorder || !sharedRecorder->getWavData())
    {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "audio/wav");
    httpd_resp_send(req, reinterpret_cast<const char*>(sharedRecorder->getWavData()),
                    sharedRecorder->getSize());
    ESP_LOGI(Tag, "WAV отправлен (%u байт)", sharedRecorder->getSize());
    return ESP_OK;
}

bool HttpServer::start()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn   = httpd_uri_match_wildcard;

    if (httpd_start(&server, &config) != ESP_OK)
    {
        ESP_LOGE(Tag, "Не удалось запустить HTTP сервер");
        return false;
    }

    httpd_uri_t wavUri = {
        .uri = "/record.wav", .method = HTTP_GET, .handler = handleWav, .user_ctx = nullptr
    };

    httpd_register_uri_handler(server, &wavUri);
    ESP_LOGI(Tag, "Сервер слушает на /record.wav");
    return true;
}

void HttpServer::stop()
{
    if (server)
    {
        httpd_stop(server);
        server = nullptr;
    }
}
