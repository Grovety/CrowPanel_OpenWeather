#pragma once
#include "adapters/lvgl/FlexContainer.h"
#include "adapters/lvgl/SimpleLabel.h"
#include "adapters/lvgl/ExpandableBlock.h"
#include "adapters/lvgl/MenuPage.h"
#include "lvgl.h"
#include "entities/Location.h"

class LocationConfiguration
{
    static constexpr char* TAG = "LocationConfig";

    FlexContainer spacer;
    MenuPage      page;
    FlexContainer contentWrapContainer;
    SimpleLabel   entryLabel;
    lv_obj_t*     textArea = nullptr;
    lv_style_t    textareaStyle;
    lv_obj_t**    keyboard = nullptr;
    char          cityText[Location::MaxCityNameLength]{};

public:
    void            create(Menu& menu, lv_obj_t** keyboard_ptr);
    void            updateCity();
    void            keyboardHandler();
    static bool     textareaCallback(lv_event_t* e);
    lv_obj_t* const getCityTextarea();
};