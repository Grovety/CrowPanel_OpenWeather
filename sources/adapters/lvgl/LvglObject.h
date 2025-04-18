#pragma once

#include "adapters/lvgl/lvgl_port_v8.h"
#include "freertos/FreeRTOS.h"

class LvglObject
{
    SemaphoreHandle_t mutex = nullptr;

protected:
    void lock()
    {
        if (!mutex)
            printf("MUTEX = NULL!!!");
        xSemaphoreTake(mutex, portMAX_DELAY);
        lvgl_port_lock();
    }

    void unlock()
    {
        lvgl_port_unlock();
        xSemaphoreGive(mutex);
    }

public:
    LvglObject()
    {
        if (!mutex)
            mutex = xSemaphoreCreateMutex();
    }
    ~LvglObject()
    {
        if (mutex)
            vSemaphoreDelete(mutex);
    }
    LvglObject(LvglObject&& other) noexcept
    {
        mutex       = other.mutex;
        other.mutex = nullptr;
    }

    LvglObject& operator=(LvglObject&& other) noexcept
    {
        if (this != &other)
        {
            if (mutex)
                vSemaphoreDelete(mutex);

            mutex       = other.mutex;
            other.mutex = nullptr;
        }
        return *this;
    }
    LvglObject(const LvglObject&)            = delete;
    LvglObject& operator=(const LvglObject&) = delete;
};