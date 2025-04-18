#pragma once

#include "adapters/lvgl/LabelBase.h"

class DescriptionLabel : LabelBase
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

    void setDescription(char* desc)
    {
        char tempText[TextLen];
        lock();
        snprintf(tempText, TextLen, "%s", desc);
        setText(tempText, true);
        unlock();
    }
};
