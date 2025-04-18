#pragma once

#include "adapters/lvgl/LvglObject.h"
#include "adapters/lvgl/SimpleLabel.h"
#include "lvgl.h"
#include <functional>
#include <cstring>

class Button : public LvglObject
{
public:
    Button() : LvglObject()
    {
        ;
    }
    ~Button()
    {
        ;
    }

    Button(const Button&)            = delete;
    Button& operator=(const Button&) = delete;

    Button(Button&& other) noexcept : LvglObject(std::move(other)),
                                      btn(other.btn),
                                      label(std::move(other.label)),
                                      event_cb(std::move(other.event_cb))
    {
        other.btn = nullptr;
    }

    Button& operator=(Button&& other) noexcept
    {
        if (this != &other)
        {
            remove();
            LvglObject::operator=(std::move(other));
            btn      = other.btn;
            label    = std::move(other.label);
            event_cb = std::move(other.event_cb);

            other.btn      = nullptr;
            other.event_cb = nullptr;
        }
        return *this;
    }
    using EventCallback = std::function<void(lv_event_t*, void*)>;

    void create(lv_obj_t* parent = nullptr, void* _context = nullptr, const char* text = nullptr,
                const lv_font_t* font = nullptr)
    {
        if (!parent)
        {
            parent = lv_scr_act();
        }

        lock();
        context = _context;
        btn     = lv_btn_create(parent);
        lv_obj_set_size(btn, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN);
        lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN);
        lv_obj_set_style_pad_all(btn, 0, LV_PART_MAIN);
        lv_obj_set_style_pad_gap(btn, 0, LV_PART_MAIN);

        if (font)
        {
            label.create(btn, font, LV_ALIGN_CENTER, 0, 0, true);
            label.setText(text);
        }
        lv_obj_set_user_data(btn, this);
        lv_obj_add_event_cb(btn, Button::event_handler, LV_EVENT_ALL, this);
        unlock();
    }

    void setText(const char* text)
    {
        if (!text)
            return;

        lock();
        label.setText(text);
        unlock();
    }

    void setEventCallback(EventCallback cb)
    {
        event_cb = cb;
    }

    lv_obj_t* get()
    {
        return btn;
    }

    void remove()
    {
        if (btn)
        {
            lv_obj_remove_event_cb(btn, Button::event_handler); // снять обработчик
            lv_obj_del(btn);
            btn = nullptr;
        }
        label.remove();
    }

private:
    lv_obj_t*     btn = nullptr;
    SimpleLabel   label;
    EventCallback event_cb = nullptr;
    void*         context  = nullptr;

    static void event_handler(lv_event_t* e)
    {
        Button* instance = static_cast<Button*>(lv_event_get_user_data(e));
        if (instance && instance->event_cb)
        {
            instance->event_cb(e, instance->context);
        }
    }
};