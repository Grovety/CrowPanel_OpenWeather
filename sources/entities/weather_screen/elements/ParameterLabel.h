#pragma once

#include "adapters/lvgl/LabelBase.h"

#define TEMPERATURE_POSTFIX "\u00B0C"
#define HUMIDITY_POSTFIX " %"
#define PRESSURE_POSTFIX " hPa"
#define WIND_SPEED_POSTFIX " m/s"

class ParameterLabel : LabelBase
{
    static constexpr uint8_t TextLen             = 50;
    static constexpr uint8_t PrefixLen           = 20;
    static constexpr uint8_t PostfixLen          = 20;
    char                     text[TextLen]       = { 0 };
    char                     prefix[PrefixLen]   = { 0 };
    char                     postfix[PostfixLen] = { 0 };

public:
    void create(lv_obj_t* parent, const lv_font_t* font, lv_align_t align = LV_ALIGN_DEFAULT,
                lv_coord_t xOffs = 0, lv_coord_t yOffs = 0)
    {
        lock();
        LabelBase::create(parent, text, sizeof(text), align, xOffs, yOffs, font, false);
        appendText("...", true);
        unlock();
    }

    void setPrefix(char* newPrefix)
    {
        if (newPrefix)
            strncpy(prefix, newPrefix, PrefixLen);
    }

    void setPostfix(char* newPostfix)
    {
        if (newPostfix)
            strncpy(postfix, newPostfix, PostfixLen);
    }

    void setParam(float param, bool printPositiveSign = false)
    {
        char tempText[TextLen];
        lock();
        snprintf(tempText, TextLen, "%s%s%d%s", prefix,
                 (((int)param > 0) && (printPositiveSign)) ? "+" : "", (int)param, postfix);
        setText(tempText, true);
        unlock();
    }
};
