#include "BrightnessBlock.h"
#include "Brightness.h"
#include <esp_log.h>

void BrightnessBlock::create(Menu& menu)
{
    page.create(menu, "Brightness");
    page.createSidebarItem("Brightness");
    /// Spacer.
    spacer.create(page.getPage(), LV_PCT(100), 20, LV_FLEX_FLOW_ROW);

    {
        /// Brightness switch.
        {
            brightnessSwitchContainer.create(page.getPage(), LV_PCT(80), LV_SIZE_CONTENT,
                                             LV_FLEX_FLOW_ROW);
            brightnessSwitchContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                                            LV_FLEX_ALIGN_CENTER);
            brightnessSwitchContainer.paddingGap(20);
            brightnessSwitchLabel.create(brightnessSwitchContainer.get(), &lv_font_montserrat_14,
                                         LV_ALIGN_LEFT_MID);
            brightnessSwitchLabel.setText("Auto:");

            brightnessSwitch = lv_switch_create(brightnessSwitchContainer.get());
            lv_obj_add_event_cb(brightnessSwitch, switchCallback, LV_EVENT_VALUE_CHANGED, this);
            /// get & set default.
            lv_obj_add_state(brightnessSwitch, LV_STATE_CHECKED);
        }

        /// Brightness slider.
        {
            brightnessSliderContainer.create(page.getPage(), LV_PCT(80), LV_SIZE_CONTENT,
                                             LV_FLEX_FLOW_ROW);
            brightnessSliderContainer.paddingGap(20);
            brightnessSliderContainer.padding(10, 0);
            brightnessSliderContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                                            LV_FLEX_ALIGN_CENTER);
            brightnessSliderContainer.paddingGap(20);
            brightnessSliderLabel.create(brightnessSliderContainer.get(), &lv_font_montserrat_14,
                                         LV_ALIGN_LEFT_MID);
            brightnessSliderLabel.setText("Manual:");
            brightnessSlider = lv_slider_create(brightnessSliderContainer.get());
            lv_obj_add_event_cb(brightnessSlider, sliderCallback, LV_EVENT_VALUE_CHANGED, this);
            lv_slider_set_value(brightnessSlider, 17, LV_ANIM_OFF);
            lv_obj_set_width(brightnessSlider, lv_pct(60));

            brightnessManualValueLabel.create(brightnessSliderContainer.get(),
                                              &lv_font_montserrat_14, LV_ALIGN_RIGHT_MID);

            lv_style_init(&brightnessSliderStyle);
            lv_style_set_opa(&brightnessSliderStyle, LV_OPA_50);
            lv_style_set_bg_color(&brightnessSliderStyle, lv_color_hex(0xAAAAAA));
            lv_style_set_border_color(&brightnessSliderStyle, lv_color_hex(0xCCCCCC));
            lv_obj_add_style(brightnessSlider, &brightnessSliderStyle,
                             LV_PART_MAIN | LV_STATE_DISABLED);

            /// get default.
            /// set default.
            brightnessManualValueLabel.setText("17%");
            setSliderDisabled(lv_obj_has_state(brightnessSwitch, LV_STATE_CHECKED));
        }
    }
}

void BrightnessBlock::setSliderDisabled(bool disabled)
{
    if (disabled)
    {
        lv_obj_add_state(brightnessSlider, LV_STATE_DISABLED);
        lv_obj_clear_flag(brightnessSlider, LV_OBJ_FLAG_CLICKABLE);
    } else
    {
        lv_obj_clear_state(brightnessSlider, LV_STATE_DISABLED);
        lv_obj_add_flag(brightnessSlider, LV_OBJ_FLAG_CLICKABLE);
    }
}

void BrightnessBlock::set(bool autoUpdate, uint8_t level)
{
    lvgl_port_lock();
    if (autoUpdate)
    {
        lv_obj_add_state(brightnessSwitch, LV_STATE_CHECKED);
    } else
    {
        lv_obj_clear_state(brightnessSwitch, LV_STATE_CHECKED);
    }

    setSliderDisabled(autoUpdate);

    lv_slider_set_value(brightnessSlider, level, LV_ANIM_OFF);
    static char textLevel[10] = { 0 };
    snprintf(textLevel, 10, "%d%%", level);
    brightnessManualValueLabel.setText(textLevel);
    lvgl_port_unlock();
}

void BrightnessBlock::switchCallback(lv_event_t* e)
{
    lvgl_port_lock();
    bool             value = lv_obj_has_state(lv_event_get_current_target(e), LV_STATE_CHECKED);
    BrightnessBlock* self  = static_cast<BrightnessBlock*>(lv_event_get_user_data(e));
    ESP_LOGI(Tag, "Auto brightness: %s", value ? "ON" : "OFF");
    self->setSliderDisabled(value);
    Brightness::instance().set(value, lv_slider_get_value(self->brightnessSlider));
    lvgl_port_unlock();
}

void BrightnessBlock::sliderCallback(lv_event_t* e)
{
    lvgl_port_lock();
    int              value = lv_slider_get_value(lv_event_get_current_target(e));
    BrightnessBlock* self  = static_cast<BrightnessBlock*>(lv_event_get_user_data(e));
    ESP_LOGI(Tag, "Brightness: %d", value);
    char text[10] = { 0 };
    snprintf(text, 10, "%d%%", value);
    self->brightnessManualValueLabel.setText(text);
    Brightness::instance().set(false, lv_slider_get_value(self->brightnessSlider));
    lvgl_port_unlock();
}