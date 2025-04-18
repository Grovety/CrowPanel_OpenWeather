#pragma once

#include "adapters/lvgl/Button.h"
#include "adapters/lvgl/ExpandableBlock.h"
#include "adapters/lvgl/FlexContainer.h"
#include "adapters/lvgl/Image.h"
#include "adapters/lvgl/ScreenBase.h"
#include "adapters/lvgl/SimpleLabel.h"
#include "entities/WIFI.h"
#include "entities/Weather.h"
#include "entities/wifi_screen/WifiScreenHeader.h"
#include "entities/wifi_screen/elements/AccessPointItem.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include "entities/Units.h"

class WifiScreen : public ScreenBase
{
    FlexContainer    mainContainer;
    WifiScreenHeader header;
    FlexContainer    contentContainer;

    ExpandableBlock unitsConfiguration;
    SimpleLabel     unitsHeaderLabel;
    FlexContainer   unitsHeaderContainer;
    FlexContainer   unitsContentWrapContainer;
    FlexContainer   temperatureUnitsEntryContainer;
    SimpleLabel     temperatureUnitsEntryLabel;
    lv_obj_t*       temperatureDropDown = nullptr;
    FlexContainer   pressureUnitsEntryContainer;
    SimpleLabel     pressureUnitsEntryLabel;
    lv_obj_t*       pressureDropDown = nullptr;

    ExpandableBlock brightnessConfiguration;
    SimpleLabel     brightnessHeaderLabel;
    FlexContainer   brightnessHeaderContainer;
    FlexContainer   brightnessSwitchContainer;
    SimpleLabel     brightnessSwitchLabel;
    lv_obj_t*       brightnessSwitch = nullptr;
    FlexContainer   brightnessSliderContainer;
    SimpleLabel     brightnessSliderLabel;
    lv_style_t      brightnessSliderStyle;
    lv_obj_t*       brightnessSlider = nullptr;
    SimpleLabel     brightnessManualValueLabel;

    ExpandableBlock cityConfiguration;
    SimpleLabel     cityHeaderLabel;
    FlexContainer   cityHeaderContainer;
    FlexContainer   cityContentWrapContainer;
    SimpleLabel     cityEntryLabel;
    lv_obj_t*       textAreaCity = nullptr;

    ExpandableBlock wifiConfiguration;
    SimpleLabel     wifiHeaderLabel;
    FlexContainer   wifiHeaderContainer;

    FlexContainer connectedListContainer;
    FlexContainer spaceContainer;
    SimpleLabel   connectedLabel;
    FlexContainer currentItemContainer;
    FlexContainer currentInfoContainer;
    FlexContainer currentDisconnectContainer;
    Image         currentWifiIcon;
    SimpleLabel   currentWifiLabel;
    Button        disconnect;
    SimpleLabel   disconnectLabel;

    FlexContainer availableListContainer;
    SimpleLabel   availableLabel;
    TimerHandle_t updateTimer;
    lv_obj_t*     textAreaPassword = nullptr;
    lv_obj_t*     keyboard         = nullptr;
    FlexContainer keyboardSpacer;

    std::vector<AccessPointItem*> wifiList;
    AccessPointItem*              currentAP;

    static constexpr char* Tag = "WifiScreen";

    static constexpr char* WifiHeaderString      = "WIFI connection";
    static constexpr char* cityHeaderString      = "Location configuration";
    static constexpr char* brighnessHeaderString = "Brightness";
    static constexpr char* unitsHeaderString     = "Units";

    static void updateTimerCallback(TimerHandle_t xTimer);

    static void disconnectButtonCallback(lv_event_t* e, void* context);

    static void wifiEventHandler(WIFI::Event event, void* context);

    void        connect(AccessPointItem* net, char* password);
    void        setBirghtnessSliderDisabled(bool disabled);
    static void passwordKeyboardEventHandler();
    static void cityKeyboardEventHandler();
    void        createUnitsBlock();
    static void unitsConfigurationHandler(lv_event_t* e);

public:
    static WifiScreen& instance()
    {
        static WifiScreen instance;
        return instance;
    }
    void create(lv_obj_t* screen_);
    void setLocation(Weather::Data& data)
    {
        header.setCity(data.city);
        header.setCountry(data.country);
        header.setGMT(data.timestampOffset);
        header.setCurrentTime(data.timestamp);
    }

    void setSSID(char* newSSID)
    {
        header.setSSID(newSSID);
    }

    void updateCurrentSSID(char* newSSID, int8_t rssi);

    void        updateWIFIList();
    void        setBrightness(bool autoUpdate, uint8_t level);
    static void connectButtonCallback(lv_event_t* e, void* context);
    static void keyboardEventCallback(lv_event_t* e);
    static void textareaEventCallback(lv_event_t* e);
    static void brightnessSwitchCallback(lv_event_t* e);
    static void brightnessSliderCallback(lv_event_t* e);
    void        setUnits(Units::Temperature temperature, Units::Pressure pressure);
};