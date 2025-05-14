#pragma once
#include "adapters/lvgl/Button.h"
#include "adapters/lvgl/ExpandableBlock.h"
#include "adapters/lvgl/FlexContainer.h"
#include "adapters/lvgl/ScreenBase.h"
#include "adapters/lvgl/SimpleLabel.h"
#include "adapters/lvgl/MenuPage.h"

class BrightnessBlock
{
    static constexpr char* Tag = "Brightness block";
    MenuPage               page;
    FlexContainer          spacer;
    FlexContainer          brightnessSwitchContainer;
    SimpleLabel            brightnessSwitchLabel;
    lv_obj_t*              brightnessSwitch = nullptr;
    FlexContainer          brightnessSliderContainer;
    SimpleLabel            brightnessSliderLabel;
    lv_style_t             brightnessSliderStyle;
    lv_obj_t*              brightnessSlider = nullptr;
    SimpleLabel            brightnessManualValueLabel;

    void setSliderDisabled(bool disabled);

public:
    void        create(Menu&);
    void        set(bool autoUpdate, uint8_t level);
    static void switchCallback(lv_event_t* e);
    static void sliderCallback(lv_event_t* e);
};