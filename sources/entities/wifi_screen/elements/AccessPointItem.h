#pragma once

#include "adapters/lvgl/Button.h"
#include "adapters/lvgl/FlexContainer.h"
#include "adapters/lvgl/Image.h"
#include "adapters/lvgl/SimpleLabel.h"
#include "adapters/lvgl/lvgl_port_v8.h"
#include "entities/WIFI.h"

struct AccessPointItem {
    FlexContainer itemContainer;
    Image         icon;
    SimpleLabel   label;
    SimpleLabel   actionLabel;
    Button        button;
    FlexContainer buttonContainer;
    FlexContainer dataContainer;

    char    ssid[MAX_SSID_LEN + 1];
    int8_t  rssi;
    uint8_t bssid[6];
    AccessPointItem(const char* s, int8_t r, uint8_t* b) : rssi(r)
    {
        strncpy(ssid, s, sizeof(ssid));
        ssid[sizeof(ssid) - 1] = '\0';
        memcpy(bssid, b, 6);
    }

    void show(lv_obj_t* parent);

    void select();

    void deselect();

    void setIcon(int8_t rssi);

    void remove();

    AccessPointItem(const AccessPointItem&)            = delete;
    AccessPointItem& operator=(const AccessPointItem&) = delete;

    AccessPointItem(AccessPointItem&& other) noexcept
        : itemContainer(std::move(other.itemContainer)),
          icon(std::move(other.icon)),
          label(std::move(other.label)),
          actionLabel(std::move(other.actionLabel)),
          button(std::move(other.button)),
          buttonContainer(std::move(other.buttonContainer)),
          dataContainer(std::move(other.dataContainer)),
          rssi(other.rssi)
    {
        strncpy(ssid, other.ssid, sizeof(ssid));
        ssid[sizeof(ssid) - 1] = '\0';
        memcpy(bssid, other.bssid, 6);
    }

    AccessPointItem& operator=(AccessPointItem&& other) noexcept
    {
        if (this != &other)
        {
            rssi = other.rssi;
            strncpy(ssid, other.ssid, sizeof(ssid));
            ssid[sizeof(ssid) - 1] = '\0';
            memcpy(bssid, other.bssid, 6);

            itemContainer   = std::move(other.itemContainer);
            icon            = std::move(other.icon);
            label           = std::move(other.label);
            actionLabel     = std::move(other.actionLabel);
            button          = std::move(other.button);
            buttonContainer = std::move(other.buttonContainer);
            dataContainer   = std::move(other.dataContainer);
        }
        return *this;
    }
};
