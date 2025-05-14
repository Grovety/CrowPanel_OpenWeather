#include "WifiScreenHeader.h"
#ifdef COMMON_DEMO_APP
#include "entities/ui/weather_screen/WeatherScreen.h"
#else
#include "entities/weather_screen/WeatherScreen.h"
#endif

LV_IMG_DECLARE(wifi_100);
LV_IMG_DECLARE(wifi_75);
LV_IMG_DECLARE(wifi_50);
LV_IMG_DECLARE(wifi_25);

LV_IMG_DECLARE(back_icon);
void WifiScreenHeader::configButtonCallback(lv_event_t* e, void* context)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        ESP_LOGI(Tag, "button pressed");
        WeatherScreen::instance().load();
    }
}

void WifiScreenHeader::create(lv_obj_t* parent)
{
    // create header.
    header.create(parent, lv_pct(100), lv_pct(8), LV_FLEX_FLOW_ROW);
    header.align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(header.get(), LV_OBJ_FLAG_SCROLLABLE);
    {
        /*******************/
        /** WIFI CONTAINER */
        /*******************/
        wifiInfoContainer.create(header.get(), lv_pct(20), lv_pct(100), LV_FLEX_FLOW_ROW);
        wifiInfoContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_clear_flag(wifiInfoContainer.get(), LV_OBJ_FLAG_SCROLLABLE);
        wifiInfoContainer.padding(5, 5);
        wifiInfoContainer.paddingGap(5);
        {
            // create wifi image.
            wifiIcon.create(wifiInfoContainer.get());
            wifiIcon.set(&wifi_25);
            wifiIcon.align(LV_ALIGN_TOP_LEFT);

            // create wifi label.
            wifiLabel.create(wifiInfoContainer.get(), &lv_font_montserrat_14);
        }

        /************************************/
        /** LOCATION CENTRAL LOCATION LABEL */
        /************************************/
        locationContainer.create(header.get(), lv_pct(60), LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW);
        lv_obj_clear_flag(locationContainer.get(), LV_OBJ_FLAG_SCROLLABLE);
        locationContainer.align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        locationContainer.paddingGap(30);
        locationContainer.padding(10, 0);
        {
            // create location label.
            location.create(locationContainer.get(), &lv_font_montserrat_16);
            timeLabel.create(locationContainer.get(), &lv_font_montserrat_16);
            dateLabel.create(locationContainer.get(), &lv_font_montserrat_16);
        }

        /*************************/
        /** CONFIGURATION BUTTON */
        /*************************/
        configButtonContainer.create(header.get(), lv_pct(20), LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW);
        configButtonContainer.align(LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        configButtonContainer.padding(0, 0);
        button.create(configButtonContainer.get());
        button.setEventCallback(configButtonCallback);
        lv_obj_set_style_pad_all(button.get(), 5, LV_PART_MAIN);
        lv_obj_clear_flag(configButtonContainer.get(), LV_OBJ_FLAG_SCROLLABLE);
        {
            // create image.
            backIcon.create(button.get());
            backIcon.scale(0.75f);
            backIcon.set(&back_icon);
            backIcon.align(LV_ALIGN_TOP_RIGHT);
        }
    }
}

void WifiScreenHeader::updateRSSI(int8_t rssi)
{
    if (rssi >= -50)
        wifiIcon.set(&wifi_100);
    else if (rssi >= -60)
        wifiIcon.set(&wifi_75);
    else if (rssi >= -70)
        wifiIcon.set(&wifi_50);
    else
        wifiIcon.set(&wifi_25);
}

void WifiScreenHeader::setSSID(char* newSSID)
{
    wifiLabel.setSSID(newSSID);
    if (!newSSID)
        wifiIcon.set(&wifi_25);
}