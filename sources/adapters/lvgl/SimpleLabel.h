#pragma once

#include "LabelBase.h"
#include <cstring>
#include "lvgl.h"

class SimpleLabel : public LabelBase
{
    static constexpr uint8_t TextLen = 100;
    char                     internalText[TextLen];

public:
    SimpleLabel() : LabelBase()
    {
        ;
    }
    ~SimpleLabel()
    {
        ;
    }

    SimpleLabel(const SimpleLabel&)            = delete;
    SimpleLabel& operator=(const SimpleLabel&) = delete;

    SimpleLabel(SimpleLabel&& other) noexcept : LabelBase(std::move(other))
    {
        memcpy(internalText, other.internalText, sizeof(internalText));
        other.label = nullptr;
    }

    SimpleLabel& operator=(SimpleLabel&& other) noexcept
    {
        if (this != &other)
        {
            this->remove();
            LabelBase::operator=(std::move(other));
            memcpy(internalText, other.internalText, sizeof(internalText));
            other.label      = nullptr;
            text             = other.text;
            maxTextLen       = other.maxTextLen;
            isAutoLock       = other.isAutoLock;
            other.text       = nullptr;
            other.maxTextLen = 0;
        }
        return *this;
    }
    void create(lv_obj_t* parent = nullptr, const lv_font_t* font = LV_FONT_DEFAULT,
                lv_align_t align = LV_ALIGN_CENTER, lv_coord_t x_ofs = 0, lv_coord_t y_ofs = 0,
                bool autoLock = true)
    {
        memset(internalText, 0, sizeof(internalText));
        LabelBase::create(parent, internalText, sizeof(internalText), align, x_ofs, y_ofs, font,
                          autoLock);
    }

    void align(lv_align_t align, lv_coord_t xOffs = 0, lv_coord_t yOffs = 0)
    {
        lv_obj_align(label, align, xOffs, yOffs);
    }

    void setText(const char* newText)
    {
        if (!newText)
            return;
        strncpy(internalText, newText, sizeof(internalText) - 1);
        internalText[sizeof(internalText) - 1] = '\0';
        LabelBase::setText(internalText, true);
    }

    lv_obj_t* get()
    {
        return label;
    }
};