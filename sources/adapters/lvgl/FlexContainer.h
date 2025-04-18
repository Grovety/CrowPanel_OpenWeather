#pragma once

#include "adapters/lvgl/LvglObject.h"

class FlexContainer : public LvglObject
{
    lv_obj_t* container = nullptr;

public:
    FlexContainer() : LvglObject()
    {
        ;
    }
    ~FlexContainer()
    {
        ;
    }
    FlexContainer(const FlexContainer&)            = delete;
    FlexContainer& operator=(const FlexContainer&) = delete;

    FlexContainer(FlexContainer&& other) noexcept : LvglObject(std::move(other)),
                                                    container(other.container)
    {
        other.container = nullptr;
    }

    FlexContainer& operator=(FlexContainer&& other) noexcept
    {
        if (this != &other)
        {
            remove();
            LvglObject::operator=(std::move(other));
            container       = other.container;
            other.container = nullptr;
        }
        return *this;
    }

    void create(lv_obj_t* parent, lv_coord_t width, lv_coord_t height, lv_flex_flow_t flow)
    {
        if (container)
            return;

        lock();

        container = lv_obj_create(parent);
        lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_size(container, width, height);
        lv_obj_set_flex_flow(container, flow);
        lv_obj_set_style_pad_all(container, 0, LV_PART_MAIN);
        lv_obj_set_style_pad_gap(container, 0, LV_PART_MAIN);
        lv_obj_set_style_border_width(container, 0, LV_PART_MAIN);
        lv_obj_set_style_shadow_width(container, 0, LV_PART_MAIN);

        unlock();
    }

    void remove()
    {
        if (!container)
            return;

        lock();
        lv_obj_del(container);
        container = nullptr;
        unlock();
    }

    void padding(lv_coord_t vertical, lv_coord_t horizontal)
    {
        if (!container)
            return;

        lock();
        lv_obj_set_style_pad_ver(container, vertical, LV_PART_MAIN);
        lv_obj_set_style_pad_hor(container, horizontal, LV_PART_MAIN);
        unlock();
    }

    void paddingGap(lv_coord_t gap)
    {
        if (!container)
            return;

        lock();
        lv_obj_set_style_pad_gap(container, gap, LV_PART_MAIN);
        unlock();
    }

    void align(lv_flex_align_t mainPlace, lv_flex_align_t crossPlace, lv_flex_align_t trackPlace)
    {
        if (!container)
            return;

        lock();
        lv_obj_set_flex_align(container, mainPlace, crossPlace, trackPlace);
        unlock();
    }

    void borderWidth(lv_coord_t width)
    {
        if (!container)
            return;

        lock();
        lv_obj_set_style_border_width(container, width, LV_PART_MAIN);
        unlock();
    }

    lv_obj_t* get()
    {
        return container;
    }
};