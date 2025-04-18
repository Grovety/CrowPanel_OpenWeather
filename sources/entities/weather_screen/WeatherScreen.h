#pragma once

#include "entities/Weather.h"
#include "entities/weather_screen/Header.h"
#include "entities/weather_screen/CurrentWeatherBody.h"
#include "entities/weather_screen/TodayForecastTable.h"
#include "entities/weather_screen/ForecastTable.h"
#include "adapters/lvgl/ScreenBase.h"

class WeatherScreen : public ScreenBase
{
    Header             header = Header(Header::ShowWifiIconOnly);
    CurrentWeatherBody currentWeatherBody;
    TodayForecastTable todayForecastTable;
    ForecastTable      forecastTable;
    FlexContainer      mainContainer;

public:
    static WeatherScreen& instance()
    {
        static WeatherScreen instance;
        return instance;
    }
    void create()
    {
        createScreen();
        lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
        mainContainer.create(screen, LV_PCT(100), LV_PCT(100), LV_FLEX_FLOW_COLUMN);
        mainContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        header.create(mainContainer);
        currentWeatherBody.create(mainContainer.get());
        todayForecastTable.create(mainContainer.get());
        forecastTable.create(mainContainer.get());
    }

    void setCurrentWeather(Weather::Data& data)
    {
        currentWeatherBody.setTemperature(data.temperature);
        currentWeatherBody.setDescription(data.description);
        currentWeatherBody.setFeelsLikeTemp(data.feelsLike);
        currentWeatherBody.setIcon(data.icon);
        currentWeatherBody.setCurrentParams(data);
        header.setCity(data.city);
        header.setCountry(data.country);
        header.setGMT(data.timestampOffset);
        header.setCurrentTime(data.timestamp);
    }

    void setForecast(Weather::Data* forecast)
    {
        todayForecastTable.setForecast(forecast);
        forecastTable.set(forecast);
    }

    void setSSID(char* newSSID)
    {
        header.setSSID(newSSID);
    }

    void updateRSSI(int8_t rssi)
    {
        header.updateRSSI(rssi);
    }
};