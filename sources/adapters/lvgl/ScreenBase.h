#pragma once

#include "adapters/lvgl/LvglObject.h"

class ScreenBase : public LvglObject
{
protected:
    lv_obj_t* screen = nullptr;

    void createScreen()
    {
        if (screen)
            return;

        lock();
        screen = lv_obj_create(NULL);
        lv_obj_set_style_bg_color(screen, lv_color_hex(0x222222), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_COVER, LV_PART_MAIN);
        unlock();
    }

    void removeScreen()
    {
        if (!screen)
            return;

        lock();
        lv_obj_del(screen);
        screen = nullptr;
        unlock();
    }

public:
    void load()
    {
        if (!screen)
            return;

        lock();
        lv_disp_load_scr(screen);
        unlock();
    }
};