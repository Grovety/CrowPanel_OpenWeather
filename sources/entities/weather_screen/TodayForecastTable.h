#pragma once

#include "adapters/lvgl/FlexContainer.h"
#include "entities/Weather.h"
#include "entities/weather_screen/elements/ParameterLabel.h"
#include "entities/weather_screen/elements/SmallTimestampLabel.h"
#include "entities/weather_screen/elements/WeatherIcon.h"
#include "entities/Units.h"
#include <esp_log.h>

class TodayForecastTable
{
    static constexpr char*   Tag                        = "TodayTable";
    static constexpr uint8_t TodayWeatherForecastsCount = 4;
    FlexContainer            mainContainer;
    FlexContainer            forecastContainer[TodayWeatherForecastsCount];
    WeatherIcon              forecastIcon[TodayWeatherForecastsCount];
    SmallTimestampLabel      timestampForecastLabel[TodayWeatherForecastsCount];
    ParameterLabel           tempSmallLabel[TodayWeatherForecastsCount];

public:
    void create(lv_obj_t* parent)
    {
        // mainContainer (row)
        mainContainer.create(parent, lv_pct(100), lv_pct(22), LV_FLEX_FLOW_ROW);
        mainContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_START);
        mainContainer.padding(10, 0);
        lv_obj_clear_flag(mainContainer.get(), LV_OBJ_FLAG_SCROLLABLE);
        {
            for (uint8_t i = 0; i < TodayWeatherForecastsCount; i++)
            {
                // forecastContainer[i] (column)
                forecastContainer[i].create(mainContainer.get(), lv_pct(25), LV_SIZE_CONTENT,
                                            LV_FLEX_FLOW_COLUMN);
                forecastContainer[i].align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                                           LV_FLEX_ALIGN_CENTER);
                {
                    /// timestampLabel
                    timestampForecastLabel[i].create(forecastContainer[i].get(),
                                                     &lv_font_montserrat_16);

                    /// icon
                    forecastIcon[i].create(forecastContainer[i].get());
                    forecastIcon[i].scale(0.5f);

                    /// smallTemperatureLabel
                    tempSmallLabel[i].create(forecastContainer[i].get(), &lv_font_montserrat_16);
                    tempSmallLabel[i].setPostfix(TEMPERATURE_POSTFIX);
                }
            }
        }
    }

    void setForecast(Weather::Data* data)
    {
        for (uint8_t i = 0; i < 4; i++)
        {
            timestampForecastLabel[i].set(data[i].timestamp);
            tempSmallLabel[i].setPostfix(
                const_cast<char*>(Units::instance().getTemperatureString()));
            tempSmallLabel[i].setParam(Units::instance().convertTemp(data[i].temperature), true);
            forecastIcon[i].set(data[i].icon);
        }
    }
};