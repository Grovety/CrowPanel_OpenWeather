#pragma once

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include <vector>
#include <array>

class WIFI
{
public:
    struct APInfo {
        char    ssid[MAX_SSID_LEN + 1];
        char    pass[MAX_PASSPHRASE_LEN];
        uint8_t bssid[6];
        bool    autoconnect;
    };
    enum Event
    {
        CONNECTED,
        CONNECT_FAIL,
        DISCONNECTED
    };
    using Callback = void(Event, void*);
    struct CallbackEntry {
        Callback* function;
        void*     userData;
    };
    WIFI();
    void         init();
    bool         connectAP(const char* ssid, uint8_t* bssid, const char* pass, bool autoconnect,
                           bool waitForConnect = false);
    bool         isConnected();
    static WIFI& instance()
    {
        static WIFI instance;
        return instance;
    }
    bool getCurrentAP(char* ssid, int8_t* rssi);
    bool disconnect();
    bool scan();
    bool getScannedAP(wifi_ap_record_t** AccessPoints, uint16_t* count);
    bool saveAP(const char* ssid, const uint8_t* bssid, const char* pass, bool autoconnect);
    bool eraseAP(const char* ssid, const uint8_t* bssid);
    bool getAP(const char* ssid, const uint8_t* bssid, char* pass, bool* autoconnect);
    bool getSavedAPs(std::vector<APInfo>& list);
    bool addCallback(Callback callback, void* userData);
    void invokeCallbacks(Event event);
    void waitForConnection();

private:
    static constexpr char    Tag[]            = "wifi";
    static constexpr uint8_t RetryCount       = 1;
    static constexpr uint8_t WifiStoreVersion = 2;

    EventGroupHandle_t           eventGroup;
    int                          retryNum = 0;
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    static void                eventHandler(void* arg, esp_event_base_t eventBase, int32_t eventId,
                                            void* eventData);
    SemaphoreHandle_t          mutex;
    wifi_ap_record_t*          scannedAP;
    uint16_t                   countScannedAp;
    char                       currSSID[MAX_SSID_LEN + 1];
    char                       currPass[MAX_PASSPHRASE_LEN];
    uint8_t                    currBSSID[6];
    bool                       currAutoconnect;
    std::vector<CallbackEntry> callbacks;

    bool getWifiStoreVersion(uint8_t* version);
};