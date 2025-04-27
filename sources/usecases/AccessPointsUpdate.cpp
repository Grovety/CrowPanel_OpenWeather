#include "AccessPointsUpdate.h"

using namespace UseCases;

bool AccessPointsUpdate::init()
{
    if (xTaskCreateWithCaps(AccessPointsUpdate::task, AccessPointsUpdate::TaskName,
                            AccessPointsUpdate::TaskSize, this, AccessPointsUpdate::TaskPriority,
                            &taskHandle, MALLOC_CAP_SPIRAM) != pdTRUE)
        return false;

    return true;
}

void AccessPointsUpdate::task(void* arg)
{
    AccessPointsUpdate* usecase       = static_cast<AccessPointsUpdate*>(arg);
    TickType_t          xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
        WifiScreen& screen             = WifiScreen::instance();
        char        ssid[MAX_SSID_LEN] = { 0 };
        int8_t      rssi               = 0;

        if (WIFI::instance().isConnected())
        {
            WIFI::instance().getCurrentAP(ssid, &rssi);
            screen.updateCurrentSSID(ssid, rssi);
        }
        wifi_ap_record_t* ap_records;
        uint16_t          ap_num;
        WIFI::instance().scan();
        WIFI::instance().getScannedAP(&ap_records, &ap_num);
        std::vector<std::string> found_ssids;
        ESP_LOGI(Tag, "found - %d", ap_num);

        std::vector<WIFI::APInfo> savedAPs;
        WIFI::instance().getSavedAPs(savedAPs);
        WIFI::APInfo* bestAutoconnect = nullptr;
        int8_t        bestRSSI        = -128;

        for (int i = 0; i < ap_num; i++)
        {
            const char* ssid  = (const char*)ap_records[i].ssid;
            int8_t      rssi  = ap_records[i].rssi;
            uint8_t*    bssid = ap_records[i].bssid;
            if (ssid[0] == '\0')
                continue;
            found_ssids.push_back(ssid);
            bool updated = false;
            for (auto& net : usecase->wifiList)
            {
                if (strcmp(net->ssid, ssid) == 0)
                {
                    net->rssi = rssi;
                    net->setIcon(rssi);
                    updated = true;
                    break;
                }
            }

            if (!updated)
            {
                void*            buffer = heap_caps_aligned_calloc(alignof(AccessPointItem), 1,
                                                                   sizeof(AccessPointItem), MALLOC_CAP_SPIRAM);
                AccessPointItem* net =
                    new (buffer) AccessPointItem(ssid, rssi, ap_records[i].bssid);
                if (net == nullptr)
                    continue;
                usecase->wifiList.emplace_back(net);
                lvgl_port_lock(-1);
                net->show(WifiScreen::instance().getAvialableAPList());
                lvgl_port_unlock();
                ESP_LOGI(Tag, "%s(BSSID: %02x:%02x:%02x:%02x:%02x:%02x), rssi - %d", net->ssid,
                         net->bssid[0], net->bssid[1], net->bssid[2], net->bssid[3], net->bssid[4],
                         net->bssid[5], net->rssi);
            }

            for (auto& ap : savedAPs)
                if ((ap.autoconnect) && (memcmp(ap.bssid, bssid, 6) == 0))
                    if (rssi > bestRSSI)
                    {
                        bestRSSI        = rssi;
                        bestAutoconnect = &ap;
                    }
        }

        for (auto& ap : savedAPs)
            ESP_LOGI(
                Tag,
                "AP (auto) SSID: %s (BSSID: %02x:%02x:%02x:%02x:%02x:%02x), auto - %d, PASS: %s",
                ap.ssid, ap.bssid[0], ap.bssid[1], ap.bssid[2], ap.bssid[3], ap.bssid[4],
                ap.bssid[5], ap.autoconnect, ap.pass);

        usecase->wifiList.erase(std::remove_if(usecase->wifiList.begin(), usecase->wifiList.end(),
                                               [&](AccessPointItem* net) {
                                                   bool found =
                                                       std::find(found_ssids.begin(),
                                                                 found_ssids.end(),
                                                                 net->ssid) != found_ssids.end();
                                                   if (!found)
                                                   {
                                                       lvgl_port_lock();
                                                       net->remove();
                                                       heap_caps_free(net);
                                                       lvgl_port_unlock();
                                                       return true;
                                                   }
                                                   return false;
                                               }),
                                usecase->wifiList.end());

        if (bestAutoconnect && !WIFI::instance().isConnected())
        {
            ESP_LOGI(Tag, "Autoconnect to best SSID: %s (rssi: %d)", bestAutoconnect->ssid,
                     bestRSSI);
            AccessPointItem* netToConnect = nullptr;
            for (auto& net : usecase->wifiList)
            {
                if (memcmp(net->bssid, bestAutoconnect->bssid, 6) == 0)
                {
                    netToConnect = net;
                    break;
                }
            }
            if (netToConnect)
            {
                lvgl_port_lock();
                WifiScreen::instance().connect(netToConnect, bestAutoconnect->pass,
                                               bestAutoconnect->autoconnect);
                lvgl_port_unlock();
            }
        }
        vTaskDelayUntil(&xLastWakeTime, UpdatePersiodMs / portTICK_PERIOD_MS);
    }
}