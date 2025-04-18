#pragma once

#include "adapters/lvgl/LabelBase.h"

class TimeZoneLabel : LabelBase
{
    static constexpr uint8_t labelTextLenght = 25;
    char                     GMT[11]         = "GMT: ....";
    char                     currentTime[11] = "";
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

    void setGMT(int32_t newGMT)
    {
        lock();
        clean(false);
        int8_t hours = (int8_t)(newGMT / 3600);
        snprintf(GMT, sizeof(GMT), "GMT: %s%d", (hours > 0) ? "+" : "", hours);
        snprintf(text, sizeof(text), "%s (%s)", currentTime, GMT);
        updateOnScreen();
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
            strftime(currentTime, sizeof(currentTime), "%H:%M", timeInfo);
        } else
            snprintf(currentTime, sizeof(currentTime), "Loading...");
        snprintf(text, sizeof(text), "%s (%s)", currentTime, GMT);
        updateOnScreen();
        unlock();
    }
};