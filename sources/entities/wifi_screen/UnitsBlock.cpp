#include "UnitsBlock.h"
#include <esp_log.h>
#include "entities/Units.h"
#ifdef COMMON_DEMO_APP
#include "entities/ui/Dashboard/Dashboard.h"
#endif

#define UNITS_NAMESPACE "units"
#define KEY_TEMP "temperature"
#define KEY_PRESS "pressure"

#ifdef COMMON_DEMO_APP
void UnitsBlock::create(Menu& menu, SensorSettings* sensor_settings)
{
    page.create(menu, "Units");
    lv_obj_t* item = page.createSidebarItem("Units");
    {
        /// Spacer.
        spacer.create(page.getPage(), LV_PCT(100), 20, LV_FLEX_FLOW_ROW);
        /// Temperature units.
        {
            temperatureUnitsEntryContainer.create(page.getPage(), LV_PCT(80), LV_SIZE_CONTENT,
                                                  LV_FLEX_FLOW_ROW);
            temperatureUnitsEntryContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                                                 LV_FLEX_ALIGN_CENTER);
            temperatureUnitsEntryContainer.paddingGap(20);
            temperatureUnitsEntryContainer.padding(10, 0);
            temperatureUnitsEntryLabel.create(temperatureUnitsEntryContainer.get(),
                                              &lv_font_montserrat_14, LV_ALIGN_LEFT_MID);
            temperatureUnitsEntryLabel.setText("Temperature:");

            temperatureDropDown = lv_dropdown_create(temperatureUnitsEntryContainer.get());
            lv_dropdown_set_options(temperatureDropDown, "Celsius\nFahrenheit");
            lv_obj_add_event_cb(
                temperatureDropDown,
                [](lv_event_t* e) {
                    auto* self = static_cast<UnitsBlock*>(lv_event_get_user_data(e));
                    self->unitsConfigurationHandler(e);
                },
                LV_EVENT_VALUE_CHANGED, this);
        }

        /// Pressure.
        {
            pressureUnitsEntryContainer.create(page.getPage(), LV_PCT(80), LV_SIZE_CONTENT,
                                               LV_FLEX_FLOW_ROW);
            pressureUnitsEntryContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                                              LV_FLEX_ALIGN_CENTER);
            pressureUnitsEntryContainer.paddingGap(20);
            pressureUnitsEntryLabel.create(pressureUnitsEntryContainer.get(),
                                           &lv_font_montserrat_14, LV_ALIGN_LEFT_MID);
            pressureUnitsEntryLabel.setText("Pressure:");

            pressureDropDown = lv_dropdown_create(pressureUnitsEntryContainer.get());
            lv_dropdown_set_options(pressureDropDown, "hPa\nmmHg");
            lv_obj_add_event_cb(
                pressureDropDown,
                [](lv_event_t* e) {
                    auto* self = static_cast<UnitsBlock*>(lv_event_get_user_data(e));
                    self->unitsConfigurationHandler(e);
                },
                LV_EVENT_VALUE_CHANGED, this);
        }
    }
}

void UnitsBlock::unitsConfigurationHandler(lv_event_t* e)
{
    lvgl_port_lock();
    const lv_obj_t* dropdown = lv_event_get_target(e);
    const uint16_t  sel      = lv_dropdown_get_selected(dropdown);
    if (dropdown == temperatureDropDown)
    {
        if (sel == 0)
        {
            sensor_settings_->temperature = Celsius;
        } else if (sel == 1)
        {
            sensor_settings_->temperature = Fahrenheit;
        }
        ESP_LOGI(Tag, "Temperature units changed to %s",
                 unit_names.at(sensor_settings_->temperature));
    } else if (dropdown == pressureDropDown)
    {
        if (sel == 0)
        {
            sensor_settings_->pressure = hPa;
        } else if (sel == 1)
        {
            sensor_settings_->pressure = mmHg;
        }
        ESP_LOGI(Tag, "Pressure units changed to %s", unit_names.at(sensor_settings_->pressure));
    }

    saveSettings();
    Dashboard::instance().updateSettings(sensor_settings_->sensor_name);
    lvgl_port_unlock();
}

void UnitsBlock::updateUnitsDropdown()
{
    lv_dropdown_set_selected(temperatureDropDown,
                             static_cast<uint16_t>(sensor_settings_->temperature));
    lv_dropdown_set_selected_highlight(temperatureDropDown, true);
    lv_dropdown_set_selected(pressureDropDown,
                             static_cast<uint16_t>(sensor_settings_->pressure) -
                                 temperature_unit_types_count - 1); // 1 stands for Pa as it
                                                                    // will not be available for
                                                                    // selection
    lv_dropdown_set_selected_highlight(pressureDropDown, true);
}

void UnitsBlock::loadSettings()
{
    nvs_handle_t handle;
    esp_err_t    err = nvs_open(UNITS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK)
        return;

    auto tempVal = static_cast<uint8_t>(UnitType::Celsius);
    if (nvs_get_u8(handle, KEY_TEMP, &tempVal) == ESP_OK)
    {
        sensor_settings_->temperature = static_cast<UnitType>(tempVal);
    }

    err = nvs_open(UNITS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK)
        return;

    auto pressVal = static_cast<uint8_t>(UnitType::hPa);
    if (nvs_get_u8(handle, KEY_PRESS, &pressVal) == ESP_OK)
    {
        sensor_settings_->pressure = static_cast<UnitType>(pressVal);
    }

    updateUnitsDropdown();
    Dashboard::instance().updateUnitNames();

    nvs_close(handle);
}

void UnitsBlock::saveSettings()
{
    nvs_handle_t handle;
    esp_err_t    err = nvs_open(UNITS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK)
        return;

    err = nvs_set_u8(handle, KEY_TEMP, static_cast<uint8_t>(sensor_settings_->temperature));
    if (err != ESP_OK)
    {
        nvs_close(handle);
        return;
    }

    err = nvs_set_u8(handle, KEY_PRESS, static_cast<uint8_t>(sensor_settings_->pressure));
    if (err != ESP_OK)
    {
        nvs_close(handle);
        return;
    }

    err = nvs_commit(handle);
    nvs_close(handle);

    if (err != ESP_OK)
        return;
}
#else

void UnitsBlock::create(Menu& menu)
{
    page.create(menu, "Units");
    lv_obj_t* item = page.createSidebarItem("Units");
    {
        /// Spacer.
        spacer.create(page.getPage(), LV_PCT(100), 20, LV_FLEX_FLOW_ROW);
        /// Temperature units.
        {
            temperatureUnitsEntryContainer.create(page.getPage(), LV_PCT(80), LV_SIZE_CONTENT,
                                                  LV_FLEX_FLOW_ROW);
            temperatureUnitsEntryContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                                                 LV_FLEX_ALIGN_CENTER);
            temperatureUnitsEntryContainer.paddingGap(20);
            temperatureUnitsEntryContainer.padding(10, 0);
            temperatureUnitsEntryLabel.create(temperatureUnitsEntryContainer.get(),
                                              &lv_font_montserrat_14, LV_ALIGN_LEFT_MID);
            temperatureUnitsEntryLabel.setText("Temperature:");

            temperatureDropDown = lv_dropdown_create(temperatureUnitsEntryContainer.get());
            lv_dropdown_set_options(temperatureDropDown, Units::getTemperatureString4Lvgl());
            lv_obj_add_event_cb(
                temperatureDropDown,
                [](lv_event_t* e) {
                    auto* self = static_cast<UnitsBlock*>(lv_event_get_user_data(e));
                    self->unitsConfigurationHandler(e);
                },
                LV_EVENT_VALUE_CHANGED, this);
        }

        /// Pressure.
        {
            pressureUnitsEntryContainer.create(page.getPage(), LV_PCT(80), LV_SIZE_CONTENT,
                                               LV_FLEX_FLOW_ROW);
            pressureUnitsEntryContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                                              LV_FLEX_ALIGN_CENTER);
            pressureUnitsEntryContainer.paddingGap(20);
            pressureUnitsEntryLabel.create(pressureUnitsEntryContainer.get(),
                                           &lv_font_montserrat_14, LV_ALIGN_LEFT_MID);
            pressureUnitsEntryLabel.setText("Pressure:");

            pressureDropDown = lv_dropdown_create(pressureUnitsEntryContainer.get());
            lv_dropdown_set_options(pressureDropDown, Units::getPressureString4Lvgl());
            lv_obj_add_event_cb(
                pressureDropDown,
                [](lv_event_t* e) {
                    auto* self = static_cast<UnitsBlock*>(lv_event_get_user_data(e));
                    self->unitsConfigurationHandler(e);
                },
                LV_EVENT_VALUE_CHANGED, this);
        }
    }
}

void UnitsBlock::unitsConfigurationHandler(lv_event_t* e)
{
    lvgl_port_lock();
    if (lv_event_get_target(e) == temperatureDropDown)
    {
        Units::instance().setTemperature(
            static_cast<Units::Temperature>(lv_dropdown_get_selected(temperatureDropDown)));
        ESP_LOGI(Tag, "Temperature units changed to %s", Units::instance().getTemperatureString());
    } else if (lv_event_get_target(e) == pressureDropDown)
    {
        Units::instance().setPressure(
            static_cast<Units::Pressure>(lv_dropdown_get_selected(pressureDropDown)));
        ESP_LOGI(Tag, "Pressure units changed to %s", Units::instance().getPressureString());
    }
    lvgl_port_unlock();
}

void UnitsBlock::setUnits(Units::Temperature temperature, Units::Pressure pressure)
{
    lv_dropdown_set_selected(temperatureDropDown, static_cast<uint16_t>(temperature));
    lv_dropdown_set_selected_highlight(temperatureDropDown, true);
    lv_dropdown_set_selected(pressureDropDown, static_cast<uint16_t>(pressure));
    lv_dropdown_set_selected_highlight(pressureDropDown, true);
}
#endif
