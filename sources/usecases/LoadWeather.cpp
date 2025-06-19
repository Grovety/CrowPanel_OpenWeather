#include "LoadWeather.h"
#include "entities/weather_screen/WeatherScreen.h"
#include "entities/wifi_screen/WifiScreen.h"
#include "entities/Weather.h"
#include "entities/Location.h"
#include "entities/CurrentTime.h"
#include "freertos/FreeRTOS.h"
#include <esp_log.h>

using namespace UseCases;

bool LoadWeather::perform()
{
    if (!forecast || !currentWeather || !temporaryLocation || !location)
    {
        ESP_LOGE(Tag, "malloc failed %p, %p, %p, %p", currentWeather, forecast, temporaryLocation,
                 location);
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
        Weather::instance().setLocation(temporaryLocation->latitude, temporaryLocation->longitude,
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
        if (!showTemporary)
            CurrentTime::instance().setTimezoneOffset(currentWeather->timestampOffset);
        WeatherScreen::instance().setCurrentWeather(*currentWeather);
        WifiScreen::instance().setLocation(*currentWeather);
        WeatherScreen::instance().setForecast(forecast);
    } else
        retVal = false;
    return retVal;
}

bool LoadWeather::setTemporaryLocation(Location::Data* location_, uint16_t periodSeconds)
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

void LoadWeather::update()
{
    if (!sema)
        return;
    xSemaphoreGive(sema);
}

void LoadWeather::timeout(uint16_t periodS)
{
    if (!sema)
        return;
    xSemaphoreTake(sema, (periodS * 1000) / portTICK_PERIOD_MS);
}