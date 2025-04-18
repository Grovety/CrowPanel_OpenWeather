#pragma once

#include "adapters/lvgl/LabelBase.h"

class WifiLabel : LabelBase
{
    static constexpr uint8_t WifiLabelTextLenght = 20;
    char                     text[WifiLabelTextLenght];

public:
    void create(lv_obj_t* parent, const lv_font_t* font, lv_align_t align = LV_ALIGN_CENTER,
                lv_coord_t xOffs = 0, lv_coord_t yOffs = 0)
    {
        lock();
        LabelBase::create(parent, text, sizeof(text), align, xOffs, yOffs, font, false);
        appendText("WIFI: ...", true);
        unlock();
    }

    void setSSID(char* newSSID)
    {
        lock();
        clean(false);
        appendText(newSSID, true);
        updateOnScreen();
        unlock();
    }
};