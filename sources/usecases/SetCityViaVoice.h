#pragma once

#include <freertos/FreeRTOS.h>

namespace UseCases
{
class SetCityViaVoice
{
    static constexpr char     Tag[] = "SetCityViaVoice";
    static void               VoiceRecordingTask(void* param);
    TaskHandle_t              task                               = nullptr;
    static constexpr uint16_t ShowLocationRequestedByVoicePeriod = 10;
    bool                      result;
    int                       seconds;
    SemaphoreHandle_t         sema = xSemaphoreCreateBinary();

public:
    static SetCityViaVoice& instance()
    {
        static SetCityViaVoice instance;
        return instance;
    }
    void start(int seconds = 3);
    void stop();
    bool isFinished(bool* result_);
};
} // namespace UseCases
