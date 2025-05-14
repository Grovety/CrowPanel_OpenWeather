#pragma once

#include "entities/Weather.h"
#include "entities/weather_screen/Header.h"
#include "entities/weather_screen/CurrentWeatherBody.h"
#include "entities/weather_screen/TodayForecastTable.h"
#include "entities/weather_screen/ForecastTable.h"
#include "adapters/lvgl/ScreenBase.h"
#include "esp_heap_caps.h"

class WeatherScreen : public ScreenBase
{
    struct UI {
        Header             header = Header(Header::ShowWifiIconOnly);
        CurrentWeatherBody currentWeatherBody;
        TodayForecastTable todayForecastTable;
        ForecastTable      forecastTable;
        FlexContainer      mainContainer;
    } * ui;

public:
    static WeatherScreen& instance()
    {
        static WeatherScreen instance;
        return instance;
    }
    void create()
    {
        createScreen();
        ui = new (heap_caps_malloc(sizeof(UI), MALLOC_CAP_SPIRAM)) UI;
        lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
        ui->mainContainer.create(screen, LV_PCT(100), LV_PCT(100), LV_FLEX_FLOW_COLUMN);
        ui->mainContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        ui->header.create(ui->mainContainer);
        ui->currentWeatherBody.create(ui->mainContainer.get());
        ui->todayForecastTable.create(ui->mainContainer.get());
        ui->forecastTable.create(ui->mainContainer.get());
    }

    void setCurrentWeather(Weather::Data& data)
    {
        ui->currentWeatherBody.setTemperature(data.temperature);
        ui->currentWeatherBody.setDescription(data.description);
        ui->currentWeatherBody.setFeelsLikeTemp(data.feelsLike);
        ui->currentWeatherBody.setIcon(data.icon);
        ui->currentWeatherBody.setCurrentParams(data);
        ui->header.setCity(data.city);
        ui->header.setCountry(data.country);
        ui->header.setCurrentTime(data.timestamp);
    }

    void setForecast(Weather::Data* forecast)
    {
        ui->todayForecastTable.setForecast(forecast);
        ui->forecastTable.set(forecast);
    }

    void setSSID(char* newSSID)
    {
        ui->header.setSSID(newSSID);
    }

    void updateRSSI(int8_t rssi)
    {
        ui->header.updateRSSI(rssi);
    }
};