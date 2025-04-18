#pragma once

#include "adapters/lvgl/Image.h"

LV_IMG_DECLARE(icon_01d);
LV_IMG_DECLARE(icon_01n);
LV_IMG_DECLARE(icon_02d);
LV_IMG_DECLARE(icon_02n);
LV_IMG_DECLARE(icon_03d);
LV_IMG_DECLARE(icon_03n);
LV_IMG_DECLARE(icon_04d);
LV_IMG_DECLARE(icon_04n);
LV_IMG_DECLARE(icon_09d);
LV_IMG_DECLARE(icon_09n);
LV_IMG_DECLARE(icon_10d);
LV_IMG_DECLARE(icon_10n);
LV_IMG_DECLARE(icon_11d);
LV_IMG_DECLARE(icon_11n);
LV_IMG_DECLARE(icon_13d);
LV_IMG_DECLARE(icon_13n);
LV_IMG_DECLARE(icon_50d);
LV_IMG_DECLARE(icon_50n);

class WeatherIcon : public Image
{
public:
    void set(char* iconStr)
    {
        if (!iconStr || !image)
            return;

        lock();
        if (strcmp(iconStr, "01d") == 0)
            lv_img_set_src(image, &icon_01d);
        else if (strcmp(iconStr, "01n") == 0)
            lv_img_set_src(image, &icon_01n);

        else if (strcmp(iconStr, "02d") == 0)
            lv_img_set_src(image, &icon_02d);
        else if (strcmp(iconStr, "02n") == 0)
            lv_img_set_src(image, &icon_02n);

        else if (strcmp(iconStr, "03d") == 0)
            lv_img_set_src(image, &icon_03d);
        else if (strcmp(iconStr, "03n") == 0)
            lv_img_set_src(image, &icon_03n);

        else if (strcmp(iconStr, "04d") == 0)
            lv_img_set_src(image, &icon_04d);
        else if (strcmp(iconStr, "04n") == 0)
            lv_img_set_src(image, &icon_04n);

        else if (strcmp(iconStr, "09d") == 0)
            lv_img_set_src(image, &icon_09d);
        else if (strcmp(iconStr, "09n") == 0)
            lv_img_set_src(image, &icon_09n);

        else if (strcmp(iconStr, "10d") == 0)
            lv_img_set_src(image, &icon_10d);
        else if (strcmp(iconStr, "10n") == 0)
            lv_img_set_src(image, &icon_10n);

        else if (strcmp(iconStr, "11d") == 0)
            lv_img_set_src(image, &icon_11d);
        else if (strcmp(iconStr, "11n") == 0)
            lv_img_set_src(image, &icon_11n);

        else if (strcmp(iconStr, "13d") == 0)
            lv_img_set_src(image, &icon_13d);
        else if (strcmp(iconStr, "13n") == 0)
            lv_img_set_src(image, &icon_13n);

        else if (strcmp(iconStr, "50d") == 0)
            lv_img_set_src(image, &icon_50d);
        else if (strcmp(iconStr, "50n") == 0)
            lv_img_set_src(image, &icon_50n);

        unlock();
    }
};
