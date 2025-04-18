#include "Brightness.h"
#include "entities/CurrentTime.h"
#include "entities/wifi_screen/WifiScreen.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

void Brightness::createMask()
{
    lvgl_port_lock();
    mask = lv_obj_create(lv_layer_top());
    lv_obj_remove_style_all(mask);
    lv_obj_set_size(mask, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(mask, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(mask, LV_OPA_TRANSP, 0);
    lv_obj_clear_flag(mask, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_move_foreground(mask);
    lvgl_port_unlock();
}

void Brightness::setMaskOpacity(uint8_t percent)
{
    if (!mask)
        return;
    if (percent > 100)
        percent = 100;
    _level = percent;

    uint8_t opa = 255 - (percent * 255 / 100);
    lvgl_port_lock();
    lv_obj_set_style_bg_opa(mask, opa, 0);
    lvgl_port_unlock();
}

bool Brightness::init()
{
    nvs_handle_t nvsHandle;
    bool         retVal = true;
    _level              = DefaultValueWhenNotConfigured;
    _autoUpdate         = true;

    if (nvs_open("storage", NVS_READWRITE, &nvsHandle) != ESP_OK)
    {
        ESP_LOGE(Tag, "error nvs_open()");
        retVal = false;
    } else
    {
        uint8_t storedLevel = 100;
        if (nvs_get_u8(nvsHandle, "brightness", &storedLevel) == ESP_OK)
        {
            _autoUpdate = false;
            _level      = storedLevel;
            ESP_LOGI(Tag, "Loaded user brightness level: %d", _level);
        } else
            ESP_LOGI(Tag, "Brightness not set, use auto, default: %d", _level);

        nvs_close(nvsHandle);
    }

    createMask();
    setMaskOpacity(_level);

    WifiScreen::instance().setBrightness(_autoUpdate, _level);

    return retVal;
}

bool Brightness::set(bool isAuto, uint8_t percent)
{
    bool retVal = true;

    _level      = percent;
    _autoUpdate = isAuto;

    if (isAuto)
    {
        nvs_handle_t nvsHandle;
        if ((nvs_open("storage", NVS_READWRITE, &nvsHandle) != ESP_OK) ||
            (nvs_erase_key(nvsHandle, "brightness") != ESP_OK) || (nvs_commit(nvsHandle) != ESP_OK))
        {
            ESP_LOGE(Tag, "Failed to erase brightness key");
            retVal = false;
        } else
            ESP_LOGI(Tag, "Switched to auto brightness mode");
        nvs_close(nvsHandle);
        update(CurrentTime::instance().isTimeSet());
    } else
    {
        nvs_handle_t nvsHandle;
        if ((nvs_open("storage", NVS_READWRITE, &nvsHandle) != ESP_OK) ||
            (nvs_set_u8(nvsHandle, "brightness", percent) != ESP_OK) ||
            (nvs_commit(nvsHandle) != ESP_OK))
        {
            ESP_LOGE(Tag, "Failed to save brightness");
            retVal = false;
        } else
        {
            ESP_LOGI(Tag, "New manully configured brightness stored");
        }
        nvs_close(nvsHandle);

        setMaskOpacity(_level);
        ESP_LOGI(Tag, "Brightness manually configured %d", _level);

        WifiScreen::instance().setBrightness(_autoUpdate, _level);
    }
    return retVal;
}

bool Brightness::update(bool timeConfigured)
{
    if (!_autoUpdate)
        return true;

    uint8_t calculatedBrightness = DefaultValueWhenNotConfigured;

    if (timeConfigured)
    {
        // 06:00–18:00 — 100%
        // 18:00–22:00 — 60%
        // 22:00–06:00 — 20%
        time_t    now = CurrentTime::instance().nowLocal();
        struct tm timeinfo;
        localtime_r(&now, &timeinfo);

        int hour = timeinfo.tm_hour;
        if (hour >= 6 && hour < 18)
        {
            calculatedBrightness = 100;
        } else if (hour >= 18 && hour < 22)
        {
            calculatedBrightness = 60;
        } else
        {
            calculatedBrightness = 20;
        }

        ESP_LOGI(Tag, "Auto brightness based on time(%02d): %d", hour, calculatedBrightness);
    } else
    {
        calculatedBrightness = DefaultValueWhenNotConfigured;
        ESP_LOGI(Tag, "Time not configured, using default: %d", calculatedBrightness);
    }

    _level = calculatedBrightness;
    setMaskOpacity(_level);
    WifiScreen::instance().setBrightness(_autoUpdate, _level);

    return true;
}

bool Brightness::get(bool* isAuto, uint8_t* percent)
{
    if (!isAuto || !percent)
    {
        ESP_LOGE(Tag, "percent pointer is null");
        return false;
    }

    *isAuto  = _autoUpdate;
    *percent = _level;

    return true;
}