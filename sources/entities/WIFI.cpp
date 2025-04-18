#include "WIFI.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"
#include "string.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_DISCONNECTED_BIT BIT1

void WIFI::eventHandler(void* arg, esp_event_base_t eventBase, int32_t eventId, void* eventData)
{
    WIFI* wifi = reinterpret_cast<WIFI*>(arg);
    if (eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    } else if (eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_DISCONNECTED)
    {
        if ((wifi->retryNum >= 0) && (wifi->retryNum < RetryCount))
        {
            esp_wifi_connect();
            wifi->retryNum++;
            ESP_LOGI(Tag, "Retry to connect to the AP");
        } else if (wifi->retryNum == RetryCount)
        {
            xEventGroupSetBits(wifi->eventGroup, WIFI_DISCONNECTED_BIT);
            wifi->invokeCallbacks(CONNECT_FAIL);
            ESP_LOGI(Tag, "Connect to the AP fail");
        } else
        {
            wifi->invokeCallbacks(DISCONNECTED);
            ESP_LOGI(Tag, "Disconnected from ap");
        }
        xEventGroupClearBits(wifi->eventGroup, WIFI_CONNECTED_BIT);
        memset(wifi->currSSID, 0, sizeof(currSSID));
        memset(wifi->currPass, 0, sizeof(currPass));
        memset(wifi->currBSSID, 0, sizeof(currBSSID));
    } else if (eventBase == IP_EVENT && eventId == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*)eventData;
        ESP_LOGI(Tag, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        wifi->retryNum = -1;
        xEventGroupSetBits(wifi->eventGroup, WIFI_CONNECTED_BIT);
        xEventGroupClearBits(wifi->eventGroup, WIFI_DISCONNECTED_BIT);
        wifi->saveAP(wifi->currSSID, wifi->currBSSID, wifi->currPass);
        wifi->invokeCallbacks(CONNECTED);
    }
}

void WIFI::invokeCallbacks(Event event)
{
    for (const auto& entry : callbacks)
    {
        if (entry.function)
            entry.function(event, entry.userData);
    }
}

WIFI::WIFI()
{
    eventGroup = xEventGroupCreate();
}

void WIFI::init()
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    uint8_t wifiStoreSavedVersion = 0;
    if (getWifiStoreVersion(&wifiStoreSavedVersion))
    {
        if (wifiStoreSavedVersion < WifiStoreVersion)
        {
            nvs_handle_t handle;
            if ((nvs_flash_erase_partition("wifi_store") == ESP_OK) &&
                (nvs_open("wifi_store", NVS_READWRITE, &handle) == ESP_OK))
            {
                nvs_set_u8(handle, "__version__", WifiStoreVersion);
                nvs_commit(handle);
                nvs_close(handle);
                ESP_LOGI(Tag, "wifi_store reset and version updated to %d", WifiStoreVersion);
            }
        }
    } else
        ESP_LOGE(Tag, "Get wifi_store version failed");
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    esp_wifi_start();

    mutex = xSemaphoreCreateMutex();
}

bool WIFI::connectAP(const char* ssid, uint8_t* bssid, const char* pass, bool waitForConnect)
{
    xSemaphoreTake(mutex, portMAX_DELAY);
    esp_wifi_stop();
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &eventHandler,
                                                        this, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                                        &eventHandler, this, &instance_got_ip));
    wifi_config_t config = {
        .sta =
            {
                .threshold = {.authmode = WIFI_AUTH_OPEN},
                .sae_pwe_h2e = WPA3_SAE_PWE_HUNT_AND_PECK,
                .sae_h2e_identifier = "",
            },
    };
    retryNum = 0;
    memcpy(config.sta.ssid, ssid, strnlen(ssid, 32) + 1);
    memcpy(config.sta.password, pass, strnlen(pass, 64) + 1);
    strncpy(currSSID, ssid, sizeof(currSSID));
    memcpy(currBSSID, bssid, sizeof(currBSSID));
    strncpy(currPass, pass, sizeof(currPass));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(Tag, "connect() ssid = %s, pass %s", ssid, pass);

    xSemaphoreGive(mutex);

    ESP_LOGI(Tag, "wifi_init_sta finished.");

    if (waitForConnect)
    {
        EventBits_t bits =
            xEventGroupWaitBits(eventGroup, WIFI_CONNECTED_BIT | WIFI_DISCONNECTED_BIT, pdFALSE,
                                pdFALSE, portMAX_DELAY);
        if (bits & WIFI_CONNECTED_BIT)
        {
            ESP_LOGI(Tag, "connected to ap SSID:%s password:%s", ssid, pass);
            return true;
        } else if (bits & WIFI_DISCONNECTED_BIT)
        {
            ESP_LOGI(Tag, "Failed to connect to SSID:%s, password:%s", ssid, pass);
            esp_wifi_stop();
        } else
            ESP_LOGE(Tag, "UNEXPECTED EVENT");
    }
    return false;
}

void WIFI::waitForConnection()
{
    xEventGroupWaitBits(eventGroup, WIFI_CONNECTED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
}

bool WIFI::isConnected()
{
    auto bits = xEventGroupWaitBits(eventGroup, WIFI_CONNECTED_BIT | WIFI_DISCONNECTED_BIT, pdFALSE,
                                    pdFALSE, 0);
    return bits & WIFI_CONNECTED_BIT ? true : false;
}

bool WIFI::getCurrentAP(char* ssid, int8_t* rssi)
{
    if (!ssid || !rssi)
        return false;
    static wifi_ap_record_t ap = { 0 };

    if (esp_wifi_sta_get_ap_info(&ap) != ESP_OK)
    {
        ssid[0] = '\0';
        return false;
    }

    memcpy(ssid, ap.ssid, strnlen((char*)ap.ssid, 32) + 1);
    *rssi = ap.rssi;
    return true;
}

bool WIFI::disconnect()
{
    bool retVal = true;
    xSemaphoreTake(mutex, portMAX_DELAY);
    if ((esp_wifi_disconnect() != ESP_OK) && ((esp_wifi_stop() != ESP_OK)))
        retVal = false;
    xSemaphoreGive(mutex);
    return retVal;
}

bool WIFI::scan()
{
    bool retVal = true;
    xSemaphoreTake(mutex, portMAX_DELAY);
    wifi_scan_config_t scan_config = { .ssid        = 0,
                                       .bssid       = 0,
                                       .channel     = 0,
                                       .show_hidden = true,
                                       .scan_type   = WIFI_SCAN_TYPE_ACTIVE,
                                       .scan_time   = { .active = { .min = 100, .max = 300 } } };

    if (esp_wifi_scan_start(&scan_config, true) != ESP_OK)
    {
        retVal = false;
        ESP_LOGE(Tag, "esp_wifi_scan_start ()");
    }
    if ((esp_wifi_scan_get_ap_num(&countScannedAp) != ESP_OK) || (countScannedAp == 0) ||
        ((scannedAP = reinterpret_cast<wifi_ap_record_t*>(
              realloc(scannedAP, (sizeof(wifi_ap_record_t) * countScannedAp)))) == nullptr) ||
        (esp_wifi_scan_get_ap_records(&countScannedAp, scannedAP) != ESP_OK) ||
        (esp_wifi_scan_stop() != ESP_OK))
    {
        retVal = false;
        ESP_LOGE(Tag, "scan() stop failed");
    }
    xSemaphoreGive(mutex);
    return retVal;
}

bool WIFI::getScannedAP(wifi_ap_record_t** AccessPoints, uint16_t* count)
{
    bool retVal = true;
    xSemaphoreTake(mutex, portMAX_DELAY);

    *AccessPoints = scannedAP;
    *count        = countScannedAp;

    xSemaphoreGive(mutex);
    return retVal;
}

static void makeNvsKey(char* outKey, size_t len, const uint8_t* bssid)
{
    snprintf(outKey, len, "ap_%02X%02X%02X%02X%02X%02X", bssid[0], bssid[1], bssid[2], bssid[3],
             bssid[4], bssid[5]);
}

bool WIFI::saveAP(const char* ssid, const uint8_t* bssid, const char* pass)
{
    if (!ssid || !bssid || !pass)
        return false;

    APInfo       ap = {};
    char         key[NVS_KEY_NAME_MAX_SIZE];
    nvs_handle_t handle;
    bool         retVal = true;
    strncpy(ap.ssid, ssid, sizeof(ap.ssid) - 1);
    strncpy(ap.pass, pass, sizeof(ap.pass) - 1);
    memcpy(ap.bssid, bssid, 6);

    makeNvsKey(key, sizeof(key), bssid);

    if ((nvs_open("wifi_store", NVS_READWRITE, &handle) != ESP_OK) ||
        (nvs_set_blob(handle, key, &ap, sizeof(APInfo)) != ESP_OK) ||
        (nvs_commit(handle) != ESP_OK))
        retVal = false;

    nvs_close(handle);

    if (!retVal)
        ESP_LOGE(Tag, "Failed to save AP: %s", ssid);
    else
        ESP_LOGI(Tag, "Saved AP: %s (key: %s) %02X:%02X:%02X:%02X:%02X:%02X", ssid, key, bssid[0],
                 bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);

    return retVal;
}

bool WIFI::getAP(const char* ssid, const uint8_t* bssid, char* pass)
{
    if (!ssid || !bssid || !pass)
        return false;

    char         key[NVS_KEY_NAME_MAX_SIZE];
    nvs_handle_t handle;
    APInfo       ap     = {};
    size_t       len    = sizeof(APInfo);
    bool         retVal = true;
    makeNvsKey(key, sizeof(key), bssid);

    if ((nvs_open("wifi_store", NVS_READWRITE, &handle) != ESP_OK) ||
        (nvs_get_blob(handle, key, &ap, &len) != ESP_OK))
        retVal = false;

    nvs_close(handle);

    if (!retVal || (strncmp(ap.ssid, ssid, sizeof(ap.ssid)) != 0))
        return false;

    strncpy(pass, ap.pass, MAX_PASSPHRASE_LEN);
    return true;
}

bool WIFI::eraseAP(const char* ssid, const uint8_t* bssid)
{
    if (!ssid || !bssid)
        return false;

    char         key[NVS_KEY_NAME_MAX_SIZE];
    nvs_handle_t handle;
    bool         retVal = true;
    makeNvsKey(key, sizeof(key), bssid);

    if ((nvs_open("wifi_store", NVS_READWRITE, &handle) != ESP_OK) ||
        (nvs_erase_key(handle, key) != ESP_OK) || (nvs_commit(handle) != ESP_OK))
        retVal = false;

    nvs_close(handle);

    if (!retVal)
        ESP_LOGE(Tag, "Failed to remove AP: %s", ssid);
    else
        ESP_LOGI(Tag, "Removed AP: %s (key: %s)", ssid, key);

    return retVal;
}

bool WIFI::getSavedAPs(std::vector<APInfo>& list)
{
    nvs_handle_t handle;
    if (nvs_open("wifi_store", NVS_READONLY, &handle) != ESP_OK)
        return false;

    nvs_iterator_t it;
    if (nvs_entry_find("nvs", "wifi_store", NVS_TYPE_BLOB, &it) != ESP_OK)
    {
        nvs_close(handle);
        return false;
    }

    while (1)
    {
        nvs_entry_info_t info;
        nvs_entry_info(it, &info);

        if (strncmp(info.key, "ap_", 3) == 0)
        {
            APInfo ap  = {};
            size_t len = sizeof(APInfo);
            if (nvs_get_blob(handle, info.key, &ap, &len) == ESP_OK)
                list.push_back(ap);
            if (len != sizeof(APInfo))
                ESP_LOGW(Tag, "Skipping NVS entry %s: size mismatch (%d != %d)", info.key, len,
                         sizeof(APInfo));
        }

        if (nvs_entry_next(&it) != ESP_OK)
            break;
    }

    nvs_release_iterator(it);
    nvs_close(handle);
    return !list.empty();
}

bool WIFI::addCallback(Callback callback, void* userData)
{
    if (callback == nullptr)
        return false;

    callbacks.push_back({ callback, userData });
    return true;
}

bool WIFI::getWifiStoreVersion(uint8_t* version)
{
    if (!version)
        return false;

    nvs_handle_t handle;

    if (nvs_open("wifi_store", NVS_READONLY, &handle) != ESP_OK)
        return false;

    esp_err_t err = nvs_get_u8(handle, "__version__", version);
    nvs_close(handle);

    if (err == ESP_ERR_NVS_NOT_FOUND)
        *version = 0;
    else if (err != ESP_OK)
        return false;

    return true;
}