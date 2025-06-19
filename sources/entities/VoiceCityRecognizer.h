#pragma once

#include "esp_heap_caps.h"
#include "entities/WavRecorder.h"

class VoiceCityRecognizer
{
    static constexpr const char* Tag        = "VoiceCityRecognizer";
    static constexpr const char* WitApiUrl  = "https://api.wit.ai/speech?v=20240520";
    static constexpr const char* WitToken   = "X4WNJUTOV754K7RPZR4USUJEDI6ZOTAH";
    static constexpr size_t      BufferSize = 16000;
    char*                        responseBuffer_ =
        static_cast<char*>(heap_caps_calloc(BufferSize, sizeof(char), MALLOC_CAP_SPIRAM));

    bool findFinalJSON(char* str, size_t len, char** out_start, size_t* out_len);

public:
    bool recognizeCity(WavRecorder& recorder, char* city, size_t cityNameLen);
};
