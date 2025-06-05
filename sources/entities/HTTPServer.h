#pragma once

#include "WavRecorder.h"
#include <esp_http_server.h>

class HttpServer
{
    static constexpr char Tag[] = "HttpServer";
    static esp_err_t      handleWav(httpd_req_t* req);

    httpd_handle_t      server = nullptr;
    static WavRecorder* sharedRecorder;

public:
    HttpServer(WavRecorder& recorder);
    ~HttpServer();

    bool start();
    void stop();
};
