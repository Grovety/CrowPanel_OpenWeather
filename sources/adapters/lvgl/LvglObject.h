#pragma once

#include "adapters/lvgl/lvgl_port_v8.h"
#include "freertos/FreeRTOS.h"

class LvglObject
{
protected:
    void lock()
    {
        lvgl_port_lock();
    }

    void unlock()
    {
        lvgl_port_unlock();
    }

public:
    LvglObject()
    {
        ;
    }
    ~LvglObject()
    {
        ;
    }
    LvglObject(LvglObject&& other) noexcept
    {
        ;
    }

    LvglObject& operator=(LvglObject&& other) noexcept
    {
        if (this != &other)
        {
            ;
        }
        return *this;
    }
    LvglObject(const LvglObject&)            = delete;
    LvglObject& operator=(const LvglObject&) = delete;
};