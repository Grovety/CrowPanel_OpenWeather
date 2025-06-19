// WavRecorder.h
#pragma once

#include <cstddef>
#include <cstdint>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "adapters/i2s_microphone/I2SMicrophone.h"

class WavRecorder
{
    static constexpr const char* Tag            = "WavRecorder";
    static constexpr int         sampleRate     = 16000;
    static constexpr int         channels       = 1;
    static constexpr int         bitsPerSample  = 16;
    static constexpr int         bytesPerSample = bitsPerSample / 8;

    int32_t* rawBuffer    = nullptr;
    size_t   rawBufferCap = 0;
    size_t   rawReadCount = 0;

    uint8_t* wavBuffer = nullptr;
    size_t   wavSize   = 0;

    volatile bool isRecording   = false;
    volatile bool stopRequested = false;

    TaskHandle_t taskHandle = nullptr;

    void writeHeader(uint8_t* buf, size_t dataSize);
    void convert32to16(const int32_t* in, int16_t* out, size_t count, float gain);

    static void   recordingTask(void* arg);
    I2SMicrophone mic;
    bool          micInited;

public:
    WavRecorder();
    ~WavRecorder();

    bool start(int seconds, float gain = 1.0f);

    void stop();

    const uint8_t* getWavData() const;

    size_t getSize() const;

    bool recording() const;
};
