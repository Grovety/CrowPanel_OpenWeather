#pragma once

#include "adapters/lvgl/LvglObject.h"

class Image : public LvglObject
{
protected:
    lv_obj_t* image = nullptr;

public:
    Image() : LvglObject()
    {
        ;
    }

    ~Image()
    {
        ;
    }

    void create(lv_obj_t* parent)
    {
        if (image)
            return;
        lock();
        image = lv_img_create(parent);
        unlock();
    }

    void remove()
    {
        if (!image)
            return;

        lock();
        lv_obj_del(image);
        image = nullptr;
        unlock();
    }

    lv_obj_t* get()
    {
        return image;
    }

    void scale(float factor)
    {
        if (!image)
            return;

        lock();
        lv_img_set_zoom(image, (uint16_t)(256.0f * factor));
        lv_img_set_size_mode(image, LV_IMG_SIZE_MODE_REAL);
        unlock();
    }

    void set(const lv_img_dsc_t* desc)
    {
        if (!desc || !image)
            return;

        lock();
        lv_img_set_src(image, desc);
        unlock();
    }

    void align(lv_align_t align, lv_coord_t offX = 0, lv_coord_t offY = 0)
    {
        if (!image)
            return;
        lock();
        lv_obj_align(image, align, offX, offY);
        unlock();
    }
    Image(Image&& other) noexcept
    {
        this->image = other.image;
        other.image = nullptr;
    }

    Image& operator=(Image&& other) noexcept
    {
        if (this != &other)
        {
            this->image = other.image;
            other.image = nullptr;
        }
        return *this;
    }

    Image(const Image&)            = delete;
    Image& operator=(const Image&) = delete;
};
