// WavRecorder.h
#pragma once

#include <cstddef>
#include <cstdint>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "adapters/i2s_microphone/I2SMicrophone.h"

class WavRecorder
{
    static constexpr char* Tag            = "WavRecorder";
    static constexpr int   sampleRate     = 16000;
    static constexpr int   channels       = 1;
    static constexpr int   bitsPerSample  = 16;
    static constexpr int   bytesPerSample = bitsPerSample / 8;

    // raw buffer для «сырых» int32_t сэмплов
    int32_t* rawBuffer    = nullptr;
    size_t   rawBufferCap = 0; // в сэмплах
    size_t   rawReadCount = 0; // сколько int32_t считано

    // wav-буфер с готовым заголовком + int16 PCM
    uint8_t* wavBuffer = nullptr;
    size_t   wavSize   = 0;

    // флаги состояния
    volatile bool isRecording   = false;
    volatile bool stopRequested = false;

    // таск FreeRTOS
    TaskHandle_t taskHandle = nullptr;

    void writeHeader(uint8_t* buf, size_t dataSize);
    void convert32to16(const int32_t* in, int16_t* out, size_t count, float gain);

    static void   recordingTask(void* arg);
    I2SMicrophone mic;
    bool          micInited;

public:
    WavRecorder();
    ~WavRecorder();

    /**
     * Запускает запись в фоне.
     * @param seconds  максимальная длина в секундах
     * @param gain     коэффициент усиления (по умолчанию 1.0f)
     * @return true, если таск запущен; false, если уже записываем или ошибка аллока
     */
    bool start(int seconds, float gain = 1.0f);

    /** Останавливает запись досрочно */
    void stop();

    const uint8_t* getWavData() const
    {
        return wavBuffer;
    }
    size_t getSize() const
    {
        return wavSize;
    }
    bool recording() const
    {
        return isRecording;
    }
};
