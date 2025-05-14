#pragma once

#include "adapters/lvgl/Button.h"
#include "adapters/lvgl/ExpandableBlock.h"
#include "adapters/lvgl/FlexContainer.h"
#include "adapters/lvgl/ScreenBase.h"
#include "adapters/lvgl/SimpleLabel.h"
#include "entities/WIFI.h"
#include "entities/Weather.h"
#ifdef COMMON_DEMO_APP
#include "entities/ui/wifi_screen/WifiScreenHeader.h"
#include "entities/ui/wifi_screen/WifiConnection.h"
#include "entities/ui/wifi_screen/LocationConfiguration.h"
#include "entities/ui/wifi_screen/BrightnessBlock.h"
#include "entities/ui/wifi_screen/UnitsBlock.h"
#include "entities/ui/wifi_screen/TimestampBlock.h"
#include "entities/ui/wifi_screen/SensorSelectionBlock.h"
#include "entities/ui/wifi_screen/elements/AccessPointItem.h"
#include "entities/ui/wifi_screen/SensorSelectionBlock.h"
#include "entities/ui/sensors_settings/SensorSettings.h"
#include "entities/BM8563.h"
#include "entities/ui/components/common.h"
#else
#include "entities/wifi_screen/WifiScreenHeader.h"
#include "entities/wifi_screen/WifiConnection.h"
#include "entities/wifi_screen/LocationConfiguration.h"
#include "entities/wifi_screen/BrightnessBlock.h"
#include "entities/wifi_screen/UnitsBlock.h"
#include "entities/wifi_screen/elements/AccessPointItem.h"
#endif
#include "entities/Units.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include "adapters/lvgl/Menu.h"

class WifiScreen : public ScreenBase
{
    struct UI {
        FlexContainer    mainContainer;
        WifiScreenHeader header;
        FlexContainer    contentContainer;

        Menu                  menu;
        BrightnessBlock       brightnessBlock;
        LocationConfiguration locationConfigurationBlock;
        UnitsBlock            unitsBlock;
#ifdef COMMON_DEMO_APP
        TimestampBlock       timestampBlock;
        SensorSelectionBlock sensorSelectionBlock;
#endif
    }* ui              = nullptr;
    lv_obj_t* keyboard = nullptr;

    static constexpr char* Tag = "WifiScreen";

    void createMenu(lv_obj_t*);

public:
    WifiConnection*    wifiConnectionBlock;
    static WifiScreen& instance()
    {
        static WifiScreen instance;
        return instance;
    }
    void create(lv_obj_t* screen_);

#ifdef COMMON_DEMO_APP
    void create(SensorSettings* sensor_settings, lv_obj_t* screen_);
    void loadSettings()
    {
        ui->unitsBlock.loadSettings();
        ui->sensorSelectionBlock.getSensorName();
    }

    void updateSensorNames(const std::vector<std::string>& sensorNames)
    {
        ui->sensorSelectionBlock.updateNames(sensorNames);
    }
#endif

    void setLocation(Weather::Data& data)
    {
        ui->header.setCity(data.city);
        ui->header.setCountry(data.country);
        ui->header.setCurrentTime(data.timestamp);
    }

    void setSSID(char* newSSID, int8_t rssi)
    {
        ui->header.setSSID(newSSID);
        wifiConnectionBlock->updateCurrentSSID(newSSID, rssi);
    }

    void setBrightness(bool autoUpdate, uint8_t level)
    {
        ui->brightnessBlock.set(autoUpdate, level);
    }
    static void keyboardEventCallback(lv_event_t* e);
    static void textareaEventCallback(lv_event_t* e);

    lv_obj_t* getAvailableWIFIList()
    {
        return wifiConnectionBlock->getAvailableAPList();
    }

    void connect(AccessPointItem* net, char* password, bool auto_connect)
    {
        wifiConnectionBlock->connect(net, password, auto_connect);
    }
#ifndef COMMON_DEMO_APP
    void setUnits(Units::Temperature temperature, Units::Pressure pressure)
    {
        ui->unitsBlock.setUnits(temperature, pressure);
    }
#endif
};