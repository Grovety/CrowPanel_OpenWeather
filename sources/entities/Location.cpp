#include "Location.h"
#include "adapters/HTTPRequest.h"
#include <esp_log.h>
#include <json_parser.h>
#include <nvs.h>
#include <nvs_flash.h>

static const char GetLocationURL[] = "http://ip-api.com/json";

bool Location::get(Location::Data& data)
{
    nvs_handle_t handle;
    size_t       size = sizeof(data.locationName);
    esp_err_t    err  = nvs_open("storage", NVS_READONLY, &handle);
    if (err == ESP_OK)
    {
        err = nvs_get_str(handle, "manual_location", data.locationName, &size);
        nvs_close(handle);

        if (err == ESP_OK)
        {
            data.latitude[0]  = '\0';
            data.longitude[0] = '\0';
            ESP_LOGI(Tag, "Using manual location: %s", data.locationName);
            return true;
        }
    }

    HTTPRequest request(const_cast<char*>(GetLocationURL), HTTP_METHOD_GET, locationResponseBuffer,
                        sizeof(locationResponseBuffer));

    jparse_ctx_t jctx;
    uint32_t     receivedLen;
    bool         retVal = true;
    float        latitude;
    float        longitude;
    if (((receivedLen = request.perform()) > 0) &&
        (json_parse_start(&jctx, locationResponseBuffer, receivedLen) == OS_SUCCESS) &&
        (json_obj_get_float(&jctx, "lat", &latitude) == OS_SUCCESS) &&
        (json_obj_get_float(&jctx, "lon", &longitude) == OS_SUCCESS) &&
        (json_obj_get_string(&jctx, "city", data.locationName, sizeof(Data::locationName)) ==
         OS_SUCCESS))
    {
        snprintf(data.latitude, sizeof(Data::latitude), "%.4f", latitude);
        snprintf(data.longitude, sizeof(Data::longitude), "%.4f", longitude);
        ESP_LOGI(Tag, "Parsed: lat: %s, lon: %s, city - %s", data.latitude, data.longitude,
                 data.locationName);
    } else
    {
        ESP_LOGE(Tag, "Parser failed: objects");
        retVal = false;
    }
    json_parse_end(&jctx);
    memset(locationResponseBuffer, 0, sizeof(locationResponseBuffer));
    return retVal;
}

bool Location::get(char* locationName)
{
    if (!locationName)
        return false;
    nvs_handle_t handle;
    size_t       size   = MaxCityNameLength;
    bool         retVal = true;
    if (nvs_open("storage", NVS_READONLY, &handle) != ESP_OK)
        return false;
    esp_err_t err = nvs_get_str(handle, "manual_location", locationName, &size);
    nvs_close(handle);

    if (err != ESP_OK)
    {
        locationName[0] = '\0';
        ESP_LOGI(Tag, "Using location auto-detection");
        retVal = false;
    } else
        ESP_LOGI(Tag, "Using manual location: %s", locationName);

    return retVal;
}

bool Location::setManual(char* const locationName)
{
    nvs_handle_t handle;

    esp_err_t err = nvs_open("storage", NVS_READWRITE, &handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(Tag, "Failed to open NVS: %s", esp_err_to_name(err));
        return false;
    }

    if (!locationName || strlen(locationName) == 0)
    {
        err = nvs_erase_key(handle, "manual_location");
        if (err == ESP_OK)
        {
            ESP_LOGI(Tag, "Manual location removed from NVS");
            nvs_commit(handle);
            nvs_close(handle);
            return true;
        } else if (err == ESP_ERR_NVS_NOT_FOUND)
        {
            ESP_LOGW(Tag, "Manual location already cleared");
            nvs_close(handle);
            return true;
        } else
        {
            ESP_LOGE(Tag, "Failed to erase manual_location: %s", esp_err_to_name(err));
            nvs_close(handle);
            return false;
        }
    }

    locationName[0] = toupper(locationName[0]);

    for (int i = 1; locationName[i] != '\0'; ++i)
        locationName[i] = tolower(locationName[i]);

    err = nvs_set_str(handle, "manual_location", locationName);
    if (err != ESP_OK)
    {
        ESP_LOGE(Tag, "Failed to set manual_location: %s", esp_err_to_name(err));
        nvs_close(handle);
        return false;
    }

    err = nvs_commit(handle);
    nvs_close(handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(Tag, "Failed to commit manual_location: %s", esp_err_to_name(err));
        return false;
    }

    ESP_LOGI(Tag, "Manual location saved: %s", locationName);
    return true;
}