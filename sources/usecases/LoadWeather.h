#pragma once

#include "entities/weather_screen/WeatherScreen.h"
#include "entities/wifi_screen/WifiScreen.h"
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
    bool          showTemporary          = false;
    TimerHandle_t temporaryLocationTimer = xTimerCreate(
        "tempLocation", 10000 / portTICK_PERIOD_MS, pdFALSE, this, [](TimerHandle_t timer) {
            static_cast<LoadWeather*>(pvTimerGetTimerID(timer))->showTemporary = false;
        });
    SemaphoreHandle_t sema = xSemaphoreCreateBinary();

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

    bool perform()
    {
        if (!forecast || !currentWeather || !temporaryLocation || !location)
        {
            ESP_LOGE(Tag, "malloc failed %p, %p, %p, %p", currentWeather, forecast,
                     temporaryLocation, location);
            return false;
        }
        bool retVal = true;
        if (!showTemporary)
        {
            Location::instance().get(*location);
            Weather::instance().setLocation(location->latitude, location->longitude,
                                            location->locationName);
        } else
        {
            Weather::instance().setLocation(temporaryLocation->latitude,
                                            temporaryLocation->longitude,
                                            temporaryLocation->locationName);
        }
        char   ssid[MAX_SSID_LEN + 1];
        int8_t rssi;
        WIFI::instance().getCurrentAP(ssid, &rssi);
        WeatherScreen::instance().updateRSSI(rssi);
        WeatherScreen::instance().setSSID(ssid);
        WifiScreen::instance().setSSID(ssid, rssi);
        if (Weather::instance().getCurrentWeather(currentWeather) &&
            Weather::instance().getForecast(forecast))
        {
            if (CurrentTime::instance().isTimeSet())
            {
                time(&curTimestamp);
                currentWeather->timestamp = curTimestamp + currentWeather->timestampOffset;
            }
            CurrentTime::instance().setTimezoneOffset(currentWeather->timestampOffset);
            WeatherScreen::instance().setCurrentWeather(*currentWeather);
            WifiScreen::instance().setLocation(*currentWeather);
            WeatherScreen::instance().setForecast(forecast);
        } else
            retVal = false;
        return retVal;
    }

    bool setTemporaryLocation(Location::Data* location_, uint16_t periodSeconds)
    {
        if (!temporaryLocation)
        {
            ESP_LOGE(Tag, "malloc failed temporaryLocation");
            return false;
        }

        if (!temporaryLocationTimer)
        {
            ESP_LOGE(Tag, "timer not created");
            return false;
        }

        if (!location_)
            return false;

        if (!periodSeconds)
            return false;

        showTemporary = true;
        xTimerChangePeriod(temporaryLocationTimer, (periodSeconds * 1000) / portTICK_PERIOD_MS, 0);
        memcpy(temporaryLocation, location_, sizeof(Location::Data));
        xTimerStart(temporaryLocationTimer, 0);
        return true;
    }

    void update()
    {
        if (!sema)
            return;
        xSemaphoreGive(sema);
    }

    void timeout(uint16_t periodS)
    {
        if (!sema)
            return;
        xSemaphoreTake(sema, (periodS * 1000) / portTICK_PERIOD_MS);
    }
};
} // namespace UseCases