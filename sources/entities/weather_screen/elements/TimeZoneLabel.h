#pragma once

#include "adapters/lvgl/LabelBase.h"

class TimeZoneLabel : LabelBase
{
    static constexpr uint8_t labelTextLenght = 30;
    char                     text[labelTextLenght];

public:
    void create(lv_obj_t* parent, const lv_font_t* font, lv_align_t align = LV_ALIGN_CENTER,
                lv_coord_t xOffs = 0, lv_coord_t yOffs = 0)
    {
        lock();
        LabelBase::create(parent, text, sizeof(text), align, xOffs, yOffs, font, false);
        appendText("Loading: ...", true);
        unlock();
    }

    void setCurrentTime(uint32_t timestamp)
    {
        lock();
        clean(false);
        if (timestamp != 0)
        {
            time_t     timestampStruct = timestamp;
            struct tm* timeInfo        = localtime(&timestampStruct);
            strftime(text, sizeof(text), "%H:%M", timeInfo);
        } else
            text[0] = '\0';
        updateOnScreen();
        unlock();
    }

    void setCurrentDate(uint32_t timestamp)
    {
        lock();
        clean(false);
        if (timestamp != 0)
        {
            time_t     timestampStruct = timestamp;
            struct tm* timeInfo        = localtime(&timestampStruct);
            strftime(text, sizeof(text), "%m.%d.%y, %a", timeInfo);
        } else
            text[0] = '\0';
        updateOnScreen();
        unlock();
    }
};