#pragma once

#include "adapters/lvgl/FlexContainer.h"
#include "entities/weather_screen/elements/DayLabel.h"
#include "entities/weather_screen/elements/ParameterLabel.h"
#include "entities/weather_screen/elements/SmallTimestampLabel.h"
#include "entities/weather_screen/elements/WeatherIcon.h"
#include <esp_log.h>

class ForecastTable
{
    static constexpr char*   Tag           = "ForecastTable";
    static constexpr uint8_t ForecastCount = 4;
    FlexContainer            objContainer;
    FlexContainer            spacerContainer;
    FlexContainer            headerContainer;
    DayLabel                 headerLabel;
    FlexContainer            mainContainer;
    FlexContainer            forecastContainer[ForecastCount];
    FlexContainer            dateContainer[ForecastCount];
    DayLabel                 dayNameLabel[ForecastCount];
    FlexContainer            weatherContainer[ForecastCount];
    FlexContainer            iconContainer[ForecastCount];
    WeatherIcon              icon[ForecastCount];
    FlexContainer            dayContainer[ForecastCount];
    ParameterLabel           dayTempLabel[ForecastCount];
    FlexContainer            nightContainer[ForecastCount];
    ParameterLabel           nightTempLabel[ForecastCount];

public:
    void create(lv_obj_t* parent)
    {
        objContainer.create(parent, LV_PCT(100), LV_SIZE_CONTENT, LV_FLEX_FLOW_COLUMN);
        lv_obj_clear_flag(objContainer.get(), LV_OBJ_FLAG_SCROLLABLE);
        objContainer.align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        spacerContainer.create(objContainer.get(), LV_PCT(100), 3, LV_FLEX_FLOW_ROW);
        headerContainer.create(objContainer.get(), LV_SIZE_CONTENT, LV_SIZE_CONTENT,
                               LV_FLEX_FLOW_COLUMN);
        headerContainer.align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        headerContainer.padding(10, 0);
        headerLabel.create(headerContainer.get(), &lv_font_montserrat_20);
        headerLabel.set("Weather forecast");
        // mainContainer (row)
        mainContainer.create(objContainer.get(), LV_PCT(100), LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW);
        for (uint8_t i = 0; i < ForecastCount; i++)
        {
            // forecastContainer[5] (row)
            forecastContainer[i].create(mainContainer.get(), LV_PCT(100 / ForecastCount),
                                        LV_SIZE_CONTENT, LV_FLEX_FLOW_COLUMN);
            forecastContainer[i].align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                                       LV_FLEX_ALIGN_CENTER);
            {
                // dateContainer (column)
                dateContainer[i].create(forecastContainer[i].get(), LV_SIZE_CONTENT,
                                        LV_SIZE_CONTENT, LV_FLEX_FLOW_COLUMN);
                dateContainer[i].align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START,
                                       LV_FLEX_ALIGN_START);
                {
                    // day name label
                    // date label
                    dayNameLabel[i].create(dateContainer[i].get(), &lv_font_montserrat_16);
                }

                // weatherContainer (row)
                weatherContainer[i].create(forecastContainer[i].get(), LV_SIZE_CONTENT,
                                           LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW);
                weatherContainer[i].align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                                          LV_FLEX_ALIGN_CENTER);
                weatherContainer[i].paddingGap(10);
                {
                    // iconContainer
                    iconContainer[i].create(weatherContainer[i].get(), LV_SIZE_CONTENT,
                                            LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW);
                    iconContainer[i].align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START,
                                           LV_FLEX_ALIGN_START);
                    {
                        // icon
                        icon[i].create(iconContainer[i].get());
                        icon[i].scale(0.5f);
                        icon[i].set("01d");
                    }

                    // dayContainer (column)
                    dayContainer[i].create(weatherContainer[i].get(), LV_SIZE_CONTENT,
                                           LV_SIZE_CONTENT, LV_FLEX_FLOW_COLUMN);
                    dayContainer[i].align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START,
                                          LV_FLEX_ALIGN_START);
                    dayContainer[i].paddingGap(5);
                    {
                        // dayLabel
                        // dayTemperature
                        dayTempLabel[i].create(dayContainer[i].get(), &lv_font_montserrat_14);
                        dayTempLabel[i].setPostfix(TEMPERATURE_POSTFIX);
                        dayTempLabel[i].setPrefix("Day ");

                        // nightLabel
                        // nightTemperature
                        nightTempLabel[i].create(dayContainer[i].get(), &lv_font_montserrat_14);
                        nightTempLabel[i].setPostfix(TEMPERATURE_POSTFIX);
                        nightTempLabel[i].setPrefix("Night ");
                    }
                }
            }
        }
    }

    void set(Weather::Data* forecast)
    {
        uint8_t idx = 0;
        for (uint8_t i = 0; i < ForecastCount; i++)
        {
            if (!Weather::instance().getNextDayIdx(forecast, &idx))
                return;
            // if (i == 0)
            //     dayNameLabel[i].set("Tomorrow");
            // else
            dayNameLabel[i].set(forecast[idx + 4].timestamp);
            icon[i].set(forecast[idx + 4].icon);

            dayTempLabel[i].setPostfix(const_cast<char*>(Units::instance().getTemperatureString()));
            dayTempLabel[i].setParam(Units::instance().convertTemp(forecast[idx + 4].temperature),
                                     true);
            nightTempLabel[i].setPostfix(
                const_cast<char*>(Units::instance().getTemperatureString()));
            nightTempLabel[i].setParam(Units::instance().convertTemp(forecast[idx + 8].temperature),
                                       true);
            ESP_LOGI(Tag, "[%d]day - %d, night - %d", i, (int)forecast[idx + 8].temperature,
                     (int)forecast[idx + 8].temperature);
        }

        ESP_LOGI(Tag, "container height - %d", lv_obj_get_content_height(objContainer.get()));
    }
};