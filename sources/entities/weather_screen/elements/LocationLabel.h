#pragma once

#include "adapters/lvgl/LabelBase.h"

class LocationLabel : LabelBase
{
    static constexpr uint8_t CityTextLenght             = 50;
    static constexpr uint8_t CountryTextLenght          = 4;
    static constexpr uint8_t LocationTextLenght         = CityTextLenght + CountryTextLenght + 10;
    char                     city[CityTextLenght]       = "Loading";
    char                     country[CountryTextLenght] = "...";
    char                     text[LocationTextLenght];

public:
    void create(lv_obj_t* parent, const lv_font_t* font, lv_align_t align = LV_ALIGN_CENTER,
                lv_coord_t xOffs = 0, lv_coord_t yOffs = 0)
    {
        lock();
        LabelBase::create(parent, text, sizeof(text), align, xOffs, yOffs, font, false);
        appendText("Loading: ...", true);
        unlock();
    }

    void setCountry(char* newCountry)
    {
        lock();
        clean(false);
        strncpy(country, newCountry, CountryTextLenght);
        snprintf(text, sizeof(text), "%s, %s", city, country);
        updateOnScreen();
        unlock();
    }

    void setCity(char* newCity)
    {
        lock();
        clean(false);
        strncpy(city, newCity, CityTextLenght);
        snprintf(text, sizeof(text), "%s, %s", city, country);
        updateOnScreen();
        unlock();
    }
};