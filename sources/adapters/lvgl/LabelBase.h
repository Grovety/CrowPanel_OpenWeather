#pragma once

#include "adapters/lvgl/LvglObject.h"

class LabelBase : public LvglObject
{
    void replaceUTF8Char(char* str, const char* utf8_seq, char replacement)
    {
        size_t seq_len = strlen(utf8_seq);
        char*  p       = NULL;

        while ((p = strstr(str, utf8_seq)) != NULL)
        {
            *p = replacement;
            memmove(p + 1, p + seq_len, strlen(p + seq_len) + 1);
        }
    }

    void updateScreen()
    {
        replaceUTF8Char(text, "\xE2\x80\x99", '\'');
        lv_label_set_text_static(label, text);
    }

protected:
    using LvglObject::LvglObject;
    char*  text;
    size_t maxTextLen;
    bool   isAutoLock = true;

    lv_obj_t* label = nullptr;

    void create(lv_obj_t* parent, char* _text, size_t _maxTextLen, lv_align_t align,
                lv_coord_t x_ofs, lv_coord_t y_ofs, const lv_font_t* font, bool _isAutoLock = true)
    {
        if (label || !font || !_text || !_maxTextLen)
            return;

        if ((isAutoLock = _isAutoLock))
            lock();

        text       = _text;
        maxTextLen = _maxTextLen;
        label      = lv_label_create(parent ? parent : lv_scr_act());
        memset(text, 0, maxTextLen);
        lv_label_set_text_static(label, text);
        lv_obj_set_style_text_font(label, font, 0);
        lv_obj_align(label, align, x_ofs, y_ofs);
        lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);

        if (isAutoLock)
            unlock();
    }

    void appendText(char* newText, bool updateOnScreen)
    {
        if (!label || !newText)
            return;

        if (isAutoLock)
            lock();
        strncat(text, newText, maxTextLen);
        if (updateOnScreen)
            updateScreen();

        if (isAutoLock)
            unlock();
    }

    void setText(char* newText, bool updateOnScreen)
    {
        if (!label || !newText)
            return;

        if (isAutoLock)
            lock();
        strncpy(text, newText, maxTextLen);
        if (updateOnScreen)
            updateScreen();

        if (isAutoLock)
            unlock();
    }

    void changeTextBuffer(char* newBuffer, size_t newMaxTextLen, bool updateOnScreen)
    {
        if (!label || !newBuffer || !newMaxTextLen)
            return;

        if (isAutoLock)
            lock();
        text       = newBuffer;
        maxTextLen = newMaxTextLen;
        if (updateOnScreen)
            updateScreen();

        if (isAutoLock)
            unlock();
    }

    void clean(bool updateOnScreen)
    {
        if (!label)
            return;

        if (isAutoLock)
            lock();
        memset(text, 0, maxTextLen);
        if (updateOnScreen)
            updateScreen();

        if (isAutoLock)
            unlock();
    }

    void updateOnScreen()
    {
        if (isAutoLock)
            lock();
        updateScreen();

        if (isAutoLock)
            unlock();
    }

public:
    void remove()
    {
        if (!label)
            return;

        if (isAutoLock)
            lock();
        lv_obj_del(label);
        label = nullptr;

        if (isAutoLock)
            unlock();
    }
};