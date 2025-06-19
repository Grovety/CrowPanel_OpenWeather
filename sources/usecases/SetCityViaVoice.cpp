#include "SetCityViaVoice.h"
#include "entities/WavRecorder.h"
#include "entities/VoiceCityRecognizer.h"
#include "entities/HttpServer.h"
#include "entities/Location.h"
#include "usecases/LoadWeather.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

using namespace UseCases;

void SetCityViaVoice::VoiceRecordingTask(void* param)
{
    auto* self    = static_cast<SetCityViaVoice*>(param);
    int   seconds = self->seconds;

    WavRecorder         recorder;
    VoiceCityRecognizer recognizer;
    Location ::Data     location = { 0 };
    if (xSemaphoreTake(self->sema, 0) != pdTRUE)
    {
        xSemaphoreGive(self->sema);
        xSemaphoreTake(self->sema, 0);
    }

    if (recorder.start(seconds))
    {
        xSemaphoreTake(self->sema, (seconds * 1000) / portTICK_PERIOD_MS);
        recorder.stop();
        ESP_LOGI(Tag, "WAV recored (%u bytes)", recorder.getSize());
        if (recognizer.recognizeCity(recorder, location.locationName, Location::MaxCityNameLength))
        {
            UseCases::LoadWeather::instance().setTemporaryLocation(
                &location, ShowLocationRequestedByVoicePeriod);
            UseCases::LoadWeather::instance().update();
            self->result = true;
        } else
        {
            ESP_LOGE(Tag, "Failed to recognize city from voice");
            self->result = false;
        }
    } else
    {
        ESP_LOGE(Tag, "Record failed");
        self->result = false;
    }

    SetCityViaVoice::instance().task = nullptr;
    vTaskDelete(NULL); // Убиваем задачу
}

void SetCityViaVoice::start(int seconds_)
{
    if (task)
        return;
    seconds = seconds_;
    xTaskCreateWithCaps(VoiceRecordingTask, "VoiceRecordTask", 4096, this, 5, &task,
                        MALLOC_CAP_SPIRAM);
}

bool SetCityViaVoice::isFinished(bool* result_)
{
    *result_ = result;
    return (task) ? false : true;
}

void SetCityViaVoice::stop()
{
    xSemaphoreGive(sema);
}