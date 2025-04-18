#include "Units.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <string.h>

#define UNITS_NAMESPACE "units"
#define KEY_TEMP "temperature"
#define KEY_PRESS "pressure"

bool Units::setTemperature(Temperature temp)
{
    nvs_handle_t handle;
    esp_err_t    err = nvs_open(UNITS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK)
        return false;

    err = nvs_set_u8(handle, KEY_TEMP, static_cast<uint8_t>(temp));
    if (err != ESP_OK)
    {
        nvs_close(handle);
        return false;
    }

    err = nvs_commit(handle);
    nvs_close(handle);

    if (err != ESP_OK)
        return false;

    temperature = temp;
    return true;
}

bool Units::setPressure(Pressure pres)
{
    nvs_handle_t handle;
    esp_err_t    err = nvs_open(UNITS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK)
        return false;

    err = nvs_set_u8(handle, KEY_PRESS, static_cast<uint8_t>(pres));
    if (err != ESP_OK)
    {
        nvs_close(handle);
        return false;
    }

    err = nvs_commit(handle);
    nvs_close(handle);

    if (err != ESP_OK)
        return false;

    pressure = pres;
    return true;
}

float Units::convertTemp(float celsium)
{
    return (temperature == Fahrenheit) ? ((celsium * 9.0f / 5.0f) + 32.0f) : celsium;
}

float Units::convertPressure(float hPa)
{
    return (pressure == mmHg) ? (hPa * 0.75006f) : hPa;
}

const char* Units::getTemperatureString()
{
    return TemperatureStr[temperature];
}

const char* Units::getPressureString()
{
    return PressureStr[pressure];
}

void Units::init()
{
    nvs_handle_t handle;
    esp_err_t    err = nvs_open(UNITS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK)
        return;

    uint8_t tempVal = static_cast<uint8_t>(DefaultTemperature);
    if (nvs_get_u8(handle, KEY_TEMP, &tempVal) == ESP_OK)
        temperature = static_cast<Temperature>(tempVal);

    uint8_t pressVal = static_cast<uint8_t>(DefaultPressure);
    if (nvs_get_u8(handle, KEY_PRESS, &pressVal) == ESP_OK)
        pressure = static_cast<Pressure>(pressVal);

    nvs_close(handle);
}