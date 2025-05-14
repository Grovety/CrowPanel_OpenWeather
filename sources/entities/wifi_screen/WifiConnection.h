#pragma once

#include "adapters/lvgl/Button.h"
#include "adapters/lvgl/ExpandableBlock.h"
#include "adapters/lvgl/FlexContainer.h"
#include "adapters/lvgl/Image.h"
#include "adapters/lvgl/ScreenBase.h"
#include "adapters/lvgl/SimpleLabel.h"
#include "adapters/lvgl/MenuPage.h"
#include "entities/WIFI.h"
#include "entities/Weather.h"
#ifdef COMMON_DEMO_APP
#include "entities/ui/wifi_screen/elements/AccessPointItem.h"
#else
#include "entities/wifi_screen/elements/AccessPointItem.h"
#endif
#include <esp_log.h>

class WifiConnection
{
    MenuPage page;

    FlexContainer spacer;
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
    lv_obj_t*     textAreaPassword    = nullptr;
    lv_obj_t*     checkboxAutoConnect = nullptr;
    FlexContainer keyboardSpacer;
    lv_obj_t**    keyboard;

    AccessPointItem* currentAP;

    static constexpr char* Tag = "WifiConnection";

    static void disconnectButtonCallback(lv_event_t* e, void* context);
    static void wifiEventHandler(WIFI::Event event, void* context);
    static void textareaEventCallback(lv_event_t* e);

public:
    void        create(Menu&, lv_obj_t**);
    void        updateCurrentSSID(char* newSSID, int8_t rssi);
    static void connectButtonCallback(lv_event_t* e, void* context);
    void        connect(AccessPointItem* net, char* password, bool auto_connect);
    lv_obj_t*   getAvailableAPList();
    static void passwordKeyboardEventHandler();
    lv_obj_t*   getPasswordTextarea();
};