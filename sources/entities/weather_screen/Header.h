#include "adapters/lvgl/Button.h"
#include "adapters/lvgl/FlexContainer.h"
#include "adapters/lvgl/Image.h"
#include "entities/weather_screen/elements/LocationLabel.h"
#include "entities/weather_screen/elements/TimeZoneLabel.h"
#include "entities/weather_screen/elements/WifiLabel.h"
#include "entities/wifi_screen/WifiScreen.h"
#include "usecases/SetCityViaVoice.h"
#include <esp_log.h>

LV_IMG_DECLARE(wifi_100);
LV_IMG_DECLARE(wifi_75);
LV_IMG_DECLARE(wifi_50);
LV_IMG_DECLARE(wifi_25);
LV_IMG_DECLARE(settings);
LV_IMG_DECLARE(microphone);

class Header
{
    WifiLabel     wifiLabel;
    LocationLabel location;
    TimeZoneLabel timeLabel;
    TimeZoneLabel dateLabel;
    FlexContainer header;
    FlexContainer wifiInfoContainer;
    FlexContainer locationContainer;
    FlexContainer configButtonContainer;
    Image         configIcon;
    Image         wifiIcon;
    Image         microphoneIcon;
    Button        microphoneButton;
    lv_style_t    stylePressed;
    lv_obj_t*     spinner = nullptr;
    enum MicState
    {
        Normal,
        Success,
        Error
    };
    MicState               micState = Normal;
    Button                 configButton;
    static constexpr char* Tag = "Header";
    static void            configButtonCallback(lv_event_t* e, void* context)
    {
        if (lv_event_get_code(e) == LV_EVENT_CLICKED)
        {
            ESP_LOGI(Tag, "button pressed");
            WifiScreen::instance().load();
        }
    }

    static void microphoneButtonCallback(lv_event_t* e, void* context)
    {
        lv_event_code_t code = lv_event_get_code(e);
        if ((code != LV_EVENT_PRESSED) && (code != LV_EVENT_RELEASED))
            return;
        ESP_LOGI(Tag, "microphone button %s", code == LV_EVENT_PRESSED ? "pressed" : "released");

        lv_timer_create(
            +[](lv_timer_t* timer) {
                auto* button = static_cast<lv_obj_t*>(timer->user_data);
                if (lv_obj_has_state(button, LV_STATE_PRESSED))
                {
                    lv_obj_add_state(button, LV_STATE_PRESSED);
                } else
                {
                    lv_obj_clear_state(button, LV_STATE_PRESSED);
                }
                lv_timer_del(timer);
            },
            0, lv_event_get_current_target(e));

        lv_obj_t* btn    = lv_event_get_current_target(e);
        auto*     header = static_cast<Header*>(context);
        if (header->micState == Success || header->micState == Error)
        {
            lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, LV_PART_MAIN);
            ESP_LOGI(Tag, "mic state - norm");
            header->micState = Normal;
        }

        if (code == LV_EVENT_PRESSED)
            UseCases::SetCityViaVoice::instance().start(5);

        if (code == LV_EVENT_RELEASED)
        {
            UseCases::SetCityViaVoice::instance().stop();
            lv_obj_clear_flag(header->spinner, LV_OBJ_FLAG_HIDDEN);

            lv_timer_create(
                +[](lv_timer_t* timer) {
                    Header* h = static_cast<Header*>(timer->user_data);
                    bool    res;
                    if (!UseCases::SetCityViaVoice::instance().isFinished(&res))
                        return;

                    lv_obj_add_flag(h->spinner, LV_OBJ_FLAG_HIDDEN);

                    lv_obj_t* button = h->microphoneButton.get();
                    if (res)
                    {
                        lv_obj_set_style_bg_color(button, lv_color_hex(0x009900), LV_PART_MAIN);
                        lv_obj_set_style_bg_opa(button, LV_OPA_COVER, LV_PART_MAIN);
                        h->micState = Success;
                        ESP_LOGI(Tag, "mic state - success");
                    } else
                    {
                        lv_obj_set_style_bg_color(button, lv_color_hex(0x990000), LV_PART_MAIN);
                        lv_obj_set_style_bg_opa(button, LV_OPA_COVER, LV_PART_MAIN);
                        h->micState = Error;
                        ESP_LOGI(Tag, "mic state - err");
                    }
                    lv_obj_refresh_style(button, LV_PART_MAIN, LV_STYLE_PROP_ANY);

                    lv_timer_t* resetTimer = lv_timer_create(
                        +[](lv_timer_t* rt) {
                            Header* h = static_cast<Header*>(rt->user_data);
                            if (h->micState == Success || h->micState == Error)
                            {
                                lv_obj_set_style_bg_opa(h->microphoneButton.get(), LV_OPA_TRANSP,
                                                        LV_PART_MAIN);
                                ESP_LOGI(Tag, "mic state - norm");
                                h->micState = Normal;
                            }
                            lv_timer_del(rt);
                        },
                        10000, h);

                    lv_timer_del(timer);
                },
                200, header);
        }
    }

public:
    enum WifiContainerConfig
    {
        ShowWifiIconAndName = 1,
        ShowWifiIconOnly
    };
    Header(WifiContainerConfig config) : wifiContainerConfig(config)
    {
        ;
    }
    void create(FlexContainer& parent)
    {
        // create header.
        header.create(parent.get(), lv_pct(100), lv_pct(8), LV_FLEX_FLOW_ROW);
        header.align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_clear_flag(header.get(), LV_OBJ_FLAG_SCROLLABLE);
        {
            /*******************/
            /** WIFI CONTAINER */
            /*******************/
            wifiInfoContainer.create(header.get(), lv_pct(20), lv_pct(100), LV_FLEX_FLOW_ROW);
            wifiInfoContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                                    LV_FLEX_ALIGN_CENTER);
            lv_obj_clear_flag(wifiInfoContainer.get(), LV_OBJ_FLAG_SCROLLABLE);
            wifiInfoContainer.padding(5, 5);
            wifiInfoContainer.paddingGap(5);
            {
                // create wifi image.
                wifiIcon.create(wifiInfoContainer.get());
                wifiIcon.set(&wifi_25);
                wifiIcon.align(LV_ALIGN_TOP_LEFT);

                // create wifi label.
                if (wifiContainerConfig != ShowWifiIconOnly)
                    wifiLabel.create(wifiInfoContainer.get(), &lv_font_montserrat_14);
            }

            /************************************/
            /** LOCATION CENTRAL LOCATION LABEL */
            /************************************/
            locationContainer.create(header.get(), lv_pct(60), lv_pct(100), LV_FLEX_FLOW_ROW);
            lv_obj_clear_flag(locationContainer.get(), LV_OBJ_FLAG_SCROLLABLE);
            locationContainer.align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                                    LV_FLEX_ALIGN_CENTER);
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
            configButtonContainer.create(header.get(), lv_pct(20), LV_SIZE_CONTENT,
                                         LV_FLEX_FLOW_ROW);
            configButtonContainer.align(LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_START,
                                        LV_FLEX_ALIGN_START);
            configButtonContainer.padding(5, 5);
            configButtonContainer.paddingGap(10);

            spinner = lv_spinner_create(configButtonContainer.get(), 1000, 60);
            lv_obj_align(spinner, LV_ALIGN_CENTER, 0, 0);
            lv_obj_add_flag(spinner, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_size(spinner, 25, 25);
            lv_obj_set_style_arc_width(spinner, 3, LV_PART_MAIN);
            lv_obj_set_style_arc_width(spinner, 5, LV_PART_INDICATOR);

            microphoneButton.create(configButtonContainer.get(), this);
            // lv_obj_set_style_pad_ver(microphoneButton.get(), 10, LV_PART_MAIN);
            lv_obj_set_style_pad_hor(microphoneButton.get(), 10, LV_PART_MAIN);
            microphoneButton.setEventCallback(microphoneButtonCallback);
            lv_style_init(&stylePressed);
            lv_style_set_bg_color(&stylePressed, lv_color_hex(0xAAAAAA));
            lv_style_set_bg_opa(&stylePressed, LV_OPA_COVER);
            lv_style_set_shadow_width(&stylePressed, 20);
            lv_style_set_shadow_color(&stylePressed, lv_color_hex(0x444444));
            lv_obj_add_style(microphoneButton.get(), &stylePressed, LV_STATE_PRESSED);
            lv_obj_clear_flag(microphoneButton.get(), LV_OBJ_FLAG_SCROLLABLE);

            {
                // create microphone image.
                microphoneIcon.create(microphoneButton.get());
                microphoneIcon.scale(0.75f);
                microphoneIcon.set(&microphone);
                microphoneIcon.align(LV_ALIGN_TOP_LEFT);
            }
            configButton.create(configButtonContainer.get());
            configButton.setEventCallback(configButtonCallback);
            lv_obj_clear_flag(configButtonContainer.get(), LV_OBJ_FLAG_SCROLLABLE);
            {
                // create image.
                configIcon.create(configButton.get());
                configIcon.scale(0.75f);
                configIcon.set(&settings);
                configIcon.align(LV_ALIGN_TOP_RIGHT);
            }
        }
    }

    void updateRSSI(int8_t rssi)
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

    void setSSID(char* newSSID)
    {
        if (wifiContainerConfig != ShowWifiIconOnly)
            wifiLabel.setSSID(newSSID);
        if (!newSSID)
            wifiIcon.set(&wifi_25);
    }

    void setCity(char* city)
    {
        location.setCity(city);
    }

    void setCountry(char* country)
    {
        location.setCountry(country);
        ESP_LOGI(Tag, "container height - %d", lv_obj_get_content_height(header.get()));
    }

    void setCurrentTime(uint32_t timestamp)
    {
        timeLabel.setCurrentTime(timestamp);
        dateLabel.setCurrentDate(timestamp);
    }

private:
    WifiContainerConfig wifiContainerConfig;
};