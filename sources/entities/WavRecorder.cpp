// WavRecorder.cpp

#include "WavRecorder.h"
#include "adapters/i2s_microphone/I2SMicrophone.h"
#include <esp_heap_caps.h>
#include <esp_log.h>
#include <string.h>

static constexpr char* Tag = "WavRecorder";

WavRecorder::WavRecorder()
{
}

WavRecorder::~WavRecorder()
{
    stop(); // убедимся, что таск завершён

    if (rawBuffer)
        heap_caps_free(rawBuffer);
    if (wavBuffer)
        heap_caps_free(wavBuffer);
}

bool WavRecorder::start(int seconds, float gain)
{
    if (isRecording || seconds <= 0)
    {
        return false;
    }

    rawBufferCap  = static_cast<size_t>(sampleRate) * seconds;
    rawReadCount  = 0;
    stopRequested = false;

    rawBuffer =
        static_cast<int32_t*>(heap_caps_malloc(rawBufferCap * sizeof(int32_t), MALLOC_CAP_SPIRAM));
    if (!rawBuffer)
    {
        ESP_LOGE(Tag, "Failed to alloc rawBuffer");
        return false;
    }

    BaseType_t res =
        xTaskCreate(recordingTask, "wav_rec_task", 4 * 1024, this, tskIDLE_PRIORITY, &taskHandle);
    if (res != pdPASS)
    {
        ESP_LOGE(Tag, "Failed to create recording task");
        heap_caps_free(rawBuffer);
        rawBuffer = nullptr;
        return false;
    }

    isRecording = true;
    return true;
}

void WavRecorder::stop()
{
    if (!isRecording)
        return;
    stopRequested    = true;
    uint32_t counter = 100;
    while (taskHandle && ((counter--) > 0))
        vTaskDelay(10);
}

void WavRecorder::recordingTask(void* arg)
{
    auto* self = static_cast<WavRecorder*>(arg);

    I2SMicrophone mic;
    bool          micInited = mic.init();
    if (!micInited)
    {
        ESP_LOGE(Tag, "Mic init failed");
        self->isRecording = false;
        self->taskHandle  = nullptr;
        vTaskDelete(nullptr);
        return;
    }

    // читаем пока не досрочная остановка и не достигнут лимит
    const size_t maxSamples = self->rawBufferCap;
    while (!self->stopRequested && self->rawReadCount < maxSamples)
    {
        size_t toReadBytes = sampleRate / 2 * sizeof(int32_t);
        size_t bytes =
            mic.read(reinterpret_cast<uint8_t*>(self->rawBuffer + self->rawReadCount), toReadBytes);
        if (bytes == 0)
            break;
        self->rawReadCount += bytes / sizeof(int32_t);
    }

    // Деинициализируем микрофон, чтобы освободить I2S
    mic.deinit();

    // ► Анализ амплитуды
    int32_t maxAbs = 0;
    for (size_t i = 0; i + 1 < self->rawReadCount; i += 2)
    {
        int32_t v = self->rawBuffer[i] >> 8;
        int32_t a = abs(v);
        if (a > maxAbs)
            maxAbs = a;
    }
    float autoGain = (maxAbs > 0) ? (32767.0f / static_cast<float>(maxAbs)) : 1.0f;

    // ► Подготовка WAV-буфера
    size_t finalSamples = self->rawReadCount / 2;
    size_t dataBytes    = finalSamples * sizeof(int16_t);
    size_t totalSize    = dataBytes + 44;

    self->wavBuffer = static_cast<uint8_t*>(heap_caps_malloc(totalSize, MALLOC_CAP_SPIRAM));
    if (!self->wavBuffer)
    {
        ESP_LOGE(Tag, "Failed to alloc wavBuffer");
        heap_caps_free(self->rawBuffer);
        self->rawBuffer   = nullptr;
        self->isRecording = false;
        self->taskHandle  = nullptr;
        vTaskDelete(nullptr);
        return;
    }
    memset(self->wavBuffer, 0, totalSize);
    self->writeHeader(self->wavBuffer, dataBytes);

    // ► Конвертация в 16-бит PCM
    int16_t* outPtr = reinterpret_cast<int16_t*>(self->wavBuffer + 44);
    self->convert32to16(self->rawBuffer, outPtr, self->rawReadCount, autoGain);
    self->wavSize = totalSize;
    ESP_LOGI(Tag, "Recording done: %u bytes", self->wavSize);

    // Финальная очистка
    heap_caps_free(self->rawBuffer);
    self->rawBuffer   = nullptr;
    self->isRecording = false;
    self->taskHandle  = nullptr;
    vTaskDelete(nullptr);
}

void WavRecorder::writeHeader(uint8_t* buf, size_t dataSize)
{
    uint32_t byteRate   = sampleRate * channels * bitsPerSample / 8;
    uint16_t blockAlign = channels * bitsPerSample / 8;

    memcpy(buf, "RIFF", 4);
    *(uint32_t*)(buf + 4) = 36 + dataSize;
    memcpy(buf + 8, "WAVEfmt ", 8);
    *(uint32_t*)(buf + 16) = 16;
    *(uint16_t*)(buf + 20) = 1;
    *(uint16_t*)(buf + 22) = channels;
    *(uint32_t*)(buf + 24) = sampleRate;
    *(uint32_t*)(buf + 28) = byteRate;
    *(uint16_t*)(buf + 32) = blockAlign;
    *(uint16_t*)(buf + 34) = bitsPerSample;
    memcpy(buf + 36, "data", 4);
    *(uint32_t*)(buf + 40) = dataSize;
}

void WavRecorder::convert32to16(const int32_t* in, int16_t* out, size_t count, float gain)
{
    size_t outIdx = 0;
    for (size_t i = 0; i + 1 < count; i += 2)
    {
        int32_t s      = in[i] >> 8;
        float   scaled = s * gain;
        if (scaled > 32767.0f)
            scaled = 32767.0f;
        if (scaled < -32768.0f)
            scaled = -32768.0f;
        out[outIdx++] = static_cast<int16_t>(scaled);
    }
}
