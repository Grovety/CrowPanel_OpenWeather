#pragma once

#include "adapters/lvgl/FlexContainer.h"
#include "entities/weather_screen/elements/DescriptionLabel.h"
#include "entities/weather_screen/elements/ParameterLabel.h"
#include "entities/weather_screen/elements/WeatherIcon.h"
#include "entities/Units.h"
#include <esp_log.h>

LV_IMG_DECLARE(wind_icon);
LV_IMG_DECLARE(humidity_icon);
LV_IMG_DECLARE(pressure_icon);
LV_IMG_DECLARE(sunrise_icon);
LV_IMG_DECLARE(sunset_icon);

class CurrentWeatherBody
{
    static constexpr char*   Tag                           = "CurrentWeatherBody";
    static constexpr uint8_t CurrentWeatherParametersCount = 3;
    enum CurrentWeatuerParams
    {
        WIND = 0,
        PRESSURE,
        HUMIDITY
    };
    FlexContainer       mainContainer;
    FlexContainer       spacerLeft;
    FlexContainer       infoContainer;
    FlexContainer       sunTimeInfoContainer;
    Image               sunInfoIcon;
    SimpleLabel         sunTimeInfoLabel;
    FlexContainer       weatherContainer;
    FlexContainer       tempInfoContainer;
    FlexContainer       descriptionContainer;
    FlexContainer       feelsLikeContainer;
    WeatherIcon         icon;
    ParameterLabel      temperatureLabel;
    DescriptionLabel    descriptionLabel;
    ParameterLabel      feelsLikeLabel;
    FlexContainer       currentParameters;
    FlexContainer       parameterContainers[CurrentWeatherParametersCount];
    Image               parameterIcons[CurrentWeatherParametersCount];
    ParameterLabel      parameterLabels[CurrentWeatherParametersCount];
    const lv_img_dsc_t* parameterIconsSrc[CurrentWeatherParametersCount]  = { &wind_icon,
                                                                              &pressure_icon,
                                                                              &humidity_icon };
    char*               parameterPostfixes[CurrentWeatherParametersCount] = { WIND_SPEED_POSTFIX,
                                                                              PRESSURE_POSTFIX,
                                                                              HUMIDITY_POSTFIX };

    FlexContainer spacerRight;

public:
    void create(lv_obj_t* parent)
    {
        /// main container (column)
        mainContainer.create(parent, lv_pct(100), lv_pct(40), LV_FLEX_FLOW_ROW);
        mainContainer.align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        spacerLeft.create(mainContainer.get(), lv_pct(12), lv_pct(100), LV_FLEX_FLOW_COLUMN);
        infoContainer.create(mainContainer.get(), lv_pct(76), lv_pct(100), LV_FLEX_FLOW_ROW);
        lv_obj_set_style_border_width(infoContainer.get(), 1, LV_PART_MAIN);
        lv_obj_set_style_border_color(infoContainer.get(), lv_color_hex(0x888888), LV_PART_MAIN);
        lv_obj_set_style_radius(infoContainer.get(), 25, LV_PART_MAIN);
        infoContainer.align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        {
            sunTimeInfoContainer.create(infoContainer.get(), lv_pct(30), lv_pct(100),
                                        LV_FLEX_FLOW_COLUMN);
            sunTimeInfoContainer.align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                                       LV_FLEX_ALIGN_CENTER);
            sunTimeInfoContainer.paddingGap(10);
            {
                sunInfoIcon.create(sunTimeInfoContainer.get());
                sunTimeInfoLabel.create(sunTimeInfoContainer.get(), &lv_font_montserrat_16,
                                        LV_ALIGN_CENTER);
            }
        }

        {
            weatherContainer.create(infoContainer.get(), lv_pct(40), LV_SIZE_CONTENT,
                                    LV_FLEX_FLOW_COLUMN);
            weatherContainer.align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                                   LV_FLEX_ALIGN_CENTER);
            { /// MAIN INFO CONTAINER (row)
                tempInfoContainer.create(weatherContainer.get(), lv_pct(100), LV_SIZE_CONTENT,
                                         LV_FLEX_FLOW_ROW);
                tempInfoContainer.align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                                        LV_FLEX_ALIGN_START);
                {
                    /// TEMPARATURE LABEL
                    temperatureLabel.create(tempInfoContainer.get(), &lv_font_montserrat_48);
                    temperatureLabel.setPostfix(TEMPERATURE_POSTFIX);
                    /// CLOUDS ICON
                    icon.create(tempInfoContainer.get());
                }

                /// DESCRIPTION CONTAINER (row)
                descriptionContainer.create(weatherContainer.get(), lv_pct(100), LV_SIZE_CONTENT,
                                            LV_FLEX_FLOW_ROW);
                descriptionContainer.align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                                           LV_FLEX_ALIGN_START);
                descriptionContainer.padding(5, 0);
                {
                    /// DESCRIPTION LABEL
                    descriptionLabel.create(descriptionContainer.get(), &lv_font_montserrat_20);
                }

                /// FEELS LIKE CONTAINER (row)
                feelsLikeContainer.create(weatherContainer.get(), lv_pct(100), LV_SIZE_CONTENT,
                                          LV_FLEX_FLOW_ROW);
                feelsLikeContainer.align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                                         LV_FLEX_ALIGN_START);
                feelsLikeContainer.padding(10, 0);
                {
                    /// FEELS LIKE LABEL
                    feelsLikeLabel.create(feelsLikeContainer.get(), &lv_font_montserrat_20);
                    feelsLikeLabel.setPrefix("Feels like: ");
                    feelsLikeLabel.setPostfix(TEMPERATURE_POSTFIX);
                }
            }
        }
        {
            // currentParameters (column)
            currentParameters.create(infoContainer.get(), lv_pct(30), LV_SIZE_CONTENT,
                                     LV_FLEX_FLOW_COLUMN);
            currentParameters.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
            currentParameters.padding(4, 10);
            currentParameters.paddingGap(10);
            {
                for (uint8_t i = 0; i < CurrentWeatherParametersCount; i++)
                {
                    parameterContainers[i].create(currentParameters.get(), LV_SIZE_CONTENT,
                                                  LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW);
                    parameterContainers[i].align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                                                 LV_FLEX_ALIGN_START);
                    parameterContainers[i].paddingGap(10);
                    {
                        // image.
                        parameterIcons[i].create(parameterContainers[i].get());
                        parameterIcons[i].set(parameterIconsSrc[i]);

                        // label.

                        parameterLabels[i].create(parameterContainers[i].get(),
                                                  &lv_font_montserrat_20);
                        parameterLabels[i].setPostfix(parameterPostfixes[i]);
                    }
                }
            }
        }

        spacerRight.create(mainContainer.get(), lv_pct(12), lv_pct(100), LV_FLEX_FLOW_COLUMN);
    }

    void setTemperature(float temp)
    {
        temperatureLabel.setPostfix(const_cast<char*>(Units::instance().getTemperatureString()));
        temperatureLabel.setParam(Units::instance().convertTemp(temp), true);
    }
    void setDescription(char* description)
    {
        descriptionLabel.setDescription(description);
    }
    void setFeelsLikeTemp(float temp)
    {
        feelsLikeLabel.setPostfix(const_cast<char*>(Units::instance().getTemperatureString()));
        feelsLikeLabel.setParam(Units::instance().convertTemp(temp), true);
    }
    void setIcon(char* iconStr)
    {
        icon.set(iconStr);
    }

    void setCurrentParams(Weather::Data& data)
    {
        parameterLabels[WIND].setParam(data.windSpeed);
        parameterLabels[PRESSURE].setPostfix(
            const_cast<char*>(Units::instance().getPressureString()));
        parameterLabels[PRESSURE].setParam(Units::instance().convertPressure(data.pressure));
        parameterLabels[HUMIDITY].setParam(data.humidity);

        char sunTimeInfo[50];
        if (data.timestamp <= data.sunriseTimestamp)
        {
            time_t     timestampStruct = data.sunriseTimestamp;
            struct tm* timeInfo        = localtime(&timestampStruct);
            strftime(sunTimeInfo, sizeof(sunTimeInfo), "Sunrise at %H:%M", timeInfo);
            sunInfoIcon.set(&sunrise_icon);
        } else
        {
            time_t     timestampStruct = data.sunsetTimestamp;
            struct tm* timeInfo        = localtime(&timestampStruct);
            strftime(sunTimeInfo, sizeof(sunTimeInfo), "Sunset at %H:%M", timeInfo);
            sunInfoIcon.set(&sunset_icon);
        }
        sunTimeInfoLabel.setText(sunTimeInfo);
    }
};