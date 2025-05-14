#include "LocationConfiguration.h"
#include "entities/WIFI.h"
#include "entities/Weather.h"
#ifdef COMMON_DEMO_APP
#include "entities/ui/wifi_screen/WifiScreen.h"
#else
#include "entities/wifi_screen/WifiScreen.h"
#endif
#include <cstring>
#include <esp_log.h>

void LocationConfiguration::create(Menu& menu, lv_obj_t** keyboard_ptr)
{
    page.create(menu, "Location configuration");
    page.createSidebarItem("Location");

    /// Spacer.
    spacer.create(page.getPage(), LV_PCT(100), 20, LV_FLEX_FLOW_ROW);

    contentWrapContainer.create(page.getPage(), LV_PCT(80), LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW);
    contentWrapContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    contentWrapContainer.paddingGap(20);

    entryLabel.create(contentWrapContainer.get(), &lv_font_montserrat_14, LV_ALIGN_LEFT_MID);
    entryLabel.setText("City:");

    textArea = lv_textarea_create(contentWrapContainer.get());
    Location::instance().get(cityText);
    if (cityText[0] == '\0')
        snprintf(cityText, sizeof(cityText), "Auto detect");
    lv_textarea_set_text(textArea, cityText);
    lv_textarea_set_one_line(textArea, true);
    lv_obj_set_width(textArea, lv_pct(60));

    lv_style_init(&textareaStyle);
    lv_style_set_text_color(&textareaStyle, lv_color_hex(0x777777));
    lv_obj_add_style(textArea, &textareaStyle, LV_PART_MAIN);
    lv_obj_add_event_cb(textArea, WifiScreen::textareaEventCallback, LV_EVENT_ALL, this);

    keyboard = keyboard_ptr;
}

void LocationConfiguration::keyboardHandler()
{
    char city[MAX_PASSPHRASE_LEN] = { 0 };
    strncpy(city, lv_textarea_get_text(textArea), sizeof(city) - 1);
    ESP_LOGI(TAG, "City: %s", city);

    if (Weather::instance().checkLocation(city))
    {
        Location::instance().setManual(city);

        lv_timer_create(
            [](lv_timer_t* t) {
                auto* self = static_cast<LocationConfiguration*>(t->user_data);
                lv_obj_set_style_border_color(self->textArea, lv_color_hex(0x009900), 0); // green
                lv_obj_set_style_border_width(self->textArea, 3, 0);
                lv_event_send(self->textArea, LV_EVENT_DEFOCUSED, NULL);
                lv_obj_clear_state(self->textArea, LV_STATE_FOCUSED);
                lv_obj_clear_state(self->textArea, LV_STATE_EDITED);
                lv_textarea_clear_selection(self->textArea);
                lv_timer_del(t);
            },
            0, this);

        lv_timer_create(
            [](lv_timer_t* t) {
                auto* self = static_cast<LocationConfiguration*>(t->user_data);
                lv_obj_set_style_border_width(self->textArea, 0, 0);
                if (!lv_obj_has_state(self->textArea, LV_STATE_FOCUSED))
                {
                    if (!Location::instance().get(self->cityText))
                        snprintf(self->cityText, sizeof(self->cityText), "Auto detect");
                    lv_textarea_set_text(self->textArea, self->cityText);
                }
                lv_timer_del(t);
            },
            2000, this);
    } else
    {
        lv_timer_create(
            [](lv_timer_t* t) {
                auto* self = static_cast<LocationConfiguration*>(t->user_data);
                lv_obj_set_style_border_color(self->textArea, lv_color_hex(0x990000), 0); // red
                lv_obj_set_style_border_width(self->textArea, 3, 0);
                lv_event_send(self->textArea, LV_EVENT_DEFOCUSED, NULL);
                lv_obj_clear_state(self->textArea, LV_STATE_FOCUSED);
                lv_obj_clear_state(self->textArea, LV_STATE_EDITED);
                lv_textarea_clear_selection(self->textArea);
                lv_timer_del(t);
            },
            0, this);

        lv_timer_create(
            [](lv_timer_t* t) {
                auto* self = static_cast<LocationConfiguration*>(t->user_data);
                if (!lv_obj_has_state(self->textArea, LV_STATE_FOCUSED))
                {
                    if (!Location::instance().get(self->cityText))
                        snprintf(self->cityText, sizeof(self->cityText), "Auto detect");
                    lv_textarea_set_text(self->textArea, self->cityText);
                }
                lv_obj_set_style_border_width(self->textArea, 0, 0);
                lv_timer_del(t);
            },
            2000, this);
    }
}
bool LocationConfiguration::textareaCallback(lv_event_t* e)
{
    lv_event_code_t code      = lv_event_get_code(e);
    auto*           self      = static_cast<LocationConfiguration*>(lv_event_get_user_data(e));
    bool            retVal    = true;
    static bool     unfocused = true;

    if (code == LV_EVENT_CLICKED || code == LV_EVENT_FOCUSED)
    {
        if (!WIFI::instance().isConnected())
        {
            lv_timer_create(
                [](lv_timer_t* t) {
                    auto* self = static_cast<LocationConfiguration*>(t->user_data);
                    lv_obj_set_style_border_color(self->textArea, lv_color_hex(0xaa6600),
                                                  0); // orange
                    lv_obj_set_style_border_width(self->textArea, 3, 0);
                    lv_textarea_set_text(self->textArea, "WIFI not connected");
                    lv_event_send(self->textArea, LV_EVENT_DEFOCUSED, NULL);
                    lv_obj_clear_state(self->textArea, LV_STATE_FOCUSED);
                    lv_obj_clear_state(self->textArea, LV_STATE_EDITED);
                    lv_textarea_clear_selection(self->textArea);
                    lv_timer_del(t);
                },
                0, self);

            lv_timer_create(
                [](lv_timer_t* t) {
                    auto* self = static_cast<LocationConfiguration*>(t->user_data);
                    if (!Location::instance().get(self->cityText))
                        snprintf(self->cityText, sizeof(self->cityText), "Auto detect");
                    lv_textarea_set_text(self->textArea, self->cityText);
                    lv_obj_set_style_border_width(self->textArea, 0, 0);
                    lv_timer_del(t);
                    ESP_LOGI(TAG, "textArea: %s", self->cityText);
                },
                3500, self);

            retVal = false;
        } else if (strncmp(self->cityText, "Auto detect", 11) == 0)
        {
            lv_timer_create(
                [](lv_timer_t* t) {
                    auto* self = static_cast<LocationConfiguration*>(t->user_data);
                    memset(self->cityText, 0, sizeof(self->cityText));
                    lv_textarea_set_text(self->textArea, self->cityText);
                    lv_timer_del(t);
                },
                0, self);
        }

        if (unfocused)
        {
            unfocused = false;
            lv_timer_create(
                [](lv_timer_t* t) {
                    auto* self = static_cast<LocationConfiguration*>(t->user_data);
                    lv_style_set_text_color(&self->textareaStyle, lv_color_hex(0xffffff));
                    lv_obj_refresh_style(self->textArea, LV_PART_MAIN, LV_STYLE_PROP_ANY);
                    lv_timer_del(t);
                },
                0, self);
        }
    } else if (code == LV_EVENT_DEFOCUSED)
    {
        if (!unfocused)
        {
            unfocused = true;
            lv_timer_create(
                [](lv_timer_t* t) {
                    auto* self = static_cast<LocationConfiguration*>(t->user_data);
                    lv_style_set_text_color(&self->textareaStyle, lv_color_hex(0x777777));
                    lv_obj_refresh_style(self->textArea, LV_PART_MAIN, LV_STYLE_PROP_ANY);
                    lv_timer_del(t);
                },
                0, self);

            if (self->keyboard && *self->keyboard)
            {
                lv_obj_remove_event_cb(*self->keyboard, WifiScreen::keyboardEventCallback);
                lv_obj_del(*self->keyboard);
                *self->keyboard = nullptr;
            }
        }
        retVal = false;
    }

    return retVal;
}

lv_obj_t* const LocationConfiguration::getCityTextarea()
{
    return const_cast<lv_obj_t* const>(textArea);
}
