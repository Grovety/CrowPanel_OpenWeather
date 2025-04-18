#pragma once

#include "adapters/lvgl/LabelBase.h"

class DayLabel : LabelBase
{
    static constexpr uint8_t TextLen = 50;
    char                     text[TextLen];

public:
    void create(lv_obj_t* parent, const lv_font_t* font, lv_align_t align = LV_ALIGN_CENTER,
                lv_coord_t xOffs = 0, lv_coord_t yOffs = 0)
    {
        lock();
        LabelBase::create(parent, text, sizeof(text), align, xOffs, yOffs, font, false);
        appendText("...", true);
        unlock();
    }

    void set(uint32_t timestamp)
    {
        lock();
        clean(false);
        if (timestamp != 0)
        {
            time_t     timestampStruct = timestamp;
            struct tm* timeInfo        = localtime(&timestampStruct);
            strftime(text, sizeof(text), "%A\n%d %B", timeInfo);
        } else
            snprintf(text, sizeof(text), "...");
        updateOnScreen();
        unlock();
    }

    void set(char* str)
    {
        if (!str)
            return;

        lock();
        strncpy(text, str, sizeof(text));
        unlock();
    }
};
