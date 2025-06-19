#pragma once

#include "entities/Weather.h"
#include "entities/Location.h"
#include "entities/CurrentTime.h"
#include "freertos/FreeRTOS.h"
#include <esp_log.h>

namespace UseCases
{
class LoadWeather
{
    static constexpr char Tag[] = "LoadWeather";
    Weather::Data*        currentWeather =
        static_cast<Weather::Data*>(heap_caps_calloc(1, sizeof(Weather::Data), MALLOC_CAP_SPIRAM));
    Weather::Data* forecast =
        static_cast<Weather::Data*>(heap_caps_calloc(40, sizeof(Weather::Data), MALLOC_CAP_SPIRAM));
    time_t          curTimestamp = 0;
    Location::Data* location     = static_cast<Location::Data*>(
        heap_caps_calloc(1, sizeof(Location::Data), MALLOC_CAP_SPIRAM));
    Location::Data* temporaryLocation = static_cast<Location::Data*>(
        heap_caps_calloc(1, sizeof(Location::Data), MALLOC_CAP_SPIRAM));
    bool              showTemporary = false;
    SemaphoreHandle_t sema          = xSemaphoreCreateBinary();

    static void temporaryLocationTimerCallback(TimerHandle_t timer)
    {
        auto* self          = static_cast<LoadWeather*>(pvTimerGetTimerID(timer));
        self->showTemporary = false;
    }
    TimerHandle_t temporaryLocationTimer = xTimerCreate(
        "tempLocation", 10000 / portTICK_PERIOD_MS, pdFALSE, this, temporaryLocationTimerCallback);

public:
    static LoadWeather& instance()
    {
        static LoadWeather instance;
        return instance;
    }
    LoadWeather(const LoadWeather&)            = delete;
    LoadWeather& operator=(const LoadWeather&) = delete;
    LoadWeather(LoadWeather&&)                 = delete;
    LoadWeather& operator=(LoadWeather&&)      = delete;
    LoadWeather()
    {
        ;
    }
    ~LoadWeather()
    {
        ;
    }

    bool perform();

    bool setTemporaryLocation(Location::Data* location_, uint16_t periodSeconds);

    void update();

    void timeout(uint16_t periodS);
};
} // namespace UseCases