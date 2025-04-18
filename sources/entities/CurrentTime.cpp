#include "CurrentTime.h"
#include "esp_log.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"

static const char *TAG = "CurrentTime";

CurrentTime &CurrentTime::instance() {
    static CurrentTime inst;
    return inst;
}

void CurrentTime::init(const char *ntpServer) {
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG(ntpServer);
    esp_netif_sntp_init(&config);
    ESP_LOGI(TAG, "SNTP initialized with server: %s", ntpServer);
}

bool CurrentTime::sync(uint32_t timeoutMs) {
    esp_err_t result = esp_netif_sntp_sync_wait(pdMS_TO_TICKS(timeoutMs));
    if (result == ESP_OK) {
        time(&_timestamp);
        ESP_LOGI(TAG, "SNTP time synced: %lld", _timestamp);
        return true;
    } else {
        ESP_LOGE(TAG, "SNTP time sync failed: %s", esp_err_to_name(result));
        _timestamp = 0;
        return false;
    }
}

bool CurrentTime::isTimeSet() const { return _timestamp > 0; }

time_t CurrentTime::now() const { return _timestamp; }

time_t CurrentTime::nowLocal() const { return _timestamp + _timezoneOffset; }

void CurrentTime::setTimezoneOffset(int32_t offsetSeconds) {
    _timezoneOffset = offsetSeconds;
}