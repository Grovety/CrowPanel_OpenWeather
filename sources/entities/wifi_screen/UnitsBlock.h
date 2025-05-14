#pragma once

#include "adapters/lvgl/Button.h"
#include "adapters/lvgl/ExpandableBlock.h"
#include "adapters/lvgl/FlexContainer.h"
#include "adapters/lvgl/ScreenBase.h"
#include "adapters/lvgl/SimpleLabel.h"
#include "adapters/lvgl/MenuPage.h"
#ifdef COMMON_DEMO_APP
#include "entities/ui/sensors_settings/SensorSettings.h"
#else
#include "entities/Units.h"
#endif

class UnitsBlock
{
    static constexpr char* Tag = "UnitsBlock";
    MenuPage               page;
    FlexContainer          spacer;
    FlexContainer          temperatureUnitsEntryContainer;
    SimpleLabel            temperatureUnitsEntryLabel;
    lv_obj_t*              temperatureDropDown = nullptr;
    FlexContainer          pressureUnitsEntryContainer;
    SimpleLabel            pressureUnitsEntryLabel;
    lv_obj_t*              pressureDropDown = nullptr;

    void unitsConfigurationHandler(lv_event_t*);
#ifdef COMMON_DEMO_APP
    SensorSettings* sensor_settings_;
    void            updateUnitsDropdown();
    void            saveSettings();
#endif

public:
#ifdef COMMON_DEMO_APP
    void create(Menu&, SensorSettings*);
    void loadSettings();
#else
    void create(Menu&);
    void setUnits(Units::Temperature temperature, Units::Pressure pressure);
#endif
};