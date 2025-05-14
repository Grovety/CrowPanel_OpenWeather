#include "WifiScreen.h"
#include "entities/Brightness.h"
#include "entities/Location.h"
#ifdef COMMON_DEMO_APP
#include "entities/ui/Dashboard/Dashboard.h"
#include "nvs.h"
#include "nvs_flash.h"
#endif
#include "esp_heap_caps.h"

#ifdef COMMON_DEMO_APP
void WifiScreen::create(SensorSettings* sensorSettings, lv_obj_t* screen_)
{
    createScreen(screen_);
#else
void WifiScreen::create(lv_obj_t* screen_)
{
    createScreen();
#endif
    ui = new (heap_caps_malloc(sizeof(UI), MALLOC_CAP_SPIRAM)) UI;
    wifiConnectionBlock =
        new (heap_caps_malloc(sizeof(WifiConnection), MALLOC_CAP_SPIRAM)) WifiConnection;
    if (!ui || !wifiConnectionBlock)
    {
        ESP_LOGE(Tag, "malloc failed");
        return;
    }
    lock();
    ui->mainContainer.create(screen, LV_PCT(100), LV_PCT(100), LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_bg_color(ui->mainContainer.get(), lv_color_hex(0x222222), LV_PART_MAIN);
    ui->mainContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    {
        /// Screen header.
        ui->header.create(ui->mainContainer.get());
        // if wifi get current == true

        ui->contentContainer.create(ui->mainContainer.get(), LV_PCT(100), LV_PCT(91),
                                    LV_FLEX_FLOW_COLUMN);
        // contentContainer.padding(0, 0);
        ui->contentContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

        /// Menu.
        ui->menu.create(ui->contentContainer.get());
        ui->menu.setRootBackButtonMode(LV_MENU_ROOT_BACK_BTN_DISABLED);
        ui->menu.setTransparentBackground();
        ui->menu.setSize(lv_pct(100), lv_pct(100));

        // Root page ((sidebar) left part on screen).
        ui->menu.createRootPage("Settings");
        // menu.createRootPage(nullptr);

#ifdef COMMON_DEMO_APP
        /*********************
         * UNITS CONFIGURATION
         */
        ui->unitsBlock.create(ui->menu, sensorSettings);

        /*********************
         * SENSOR CONFIGURATION
         */
        ui->sensorSelectionBlock.create(ui->menu, sensorSettings);

        /*********************
         * DATE AND TIME CONFIGURATION
         */
        ui->timestampBlock.create(ui->menu);
#else

        /*********************
         * UNITS CONFIGURATION
         */
        ui->unitsBlock.create(ui->menu);
#endif

        /*********************
         * BRIGHTNESS CONFIGURATION
         */
        ui->brightnessBlock.create(ui->menu);

        /********************
         * CITY CONFIGURATION
         */
        ui->locationConfigurationBlock.create(ui->menu, &keyboard);

        /******************
         * WIFI CONFIGURATION
         */
        wifiConnectionBlock->create(ui->menu, &keyboard);

        ui->menu.setSidebarPage(ui->menu.getRootPage());
        lv_obj_t* sidebar   = lv_menu_get_cur_sidebar_page(ui->menu.get());
        lv_obj_t* firstItem = lv_obj_get_child(sidebar, 2);
        lv_event_send(firstItem, LV_EVENT_CLICKED, nullptr);
    }

    unlock();
}

void WifiScreen::keyboardEventCallback(lv_event_t* e)
{
    lv_event_code_t code   = lv_event_get_code(e);
    WifiScreen*     screen = &WifiScreen::instance();
    if (code == LV_EVENT_READY)
    {
        screen->lock();
        if (lv_keyboard_get_textarea(screen->keyboard) ==
            screen->wifiConnectionBlock->getPasswordTextarea())
            screen->wifiConnectionBlock->passwordKeyboardEventHandler();
        else if (lv_keyboard_get_textarea(screen->keyboard) ==
                 screen->ui->locationConfigurationBlock.getCityTextarea())
            screen->ui->locationConfigurationBlock.keyboardHandler();
        screen->unlock();
    }
    if ((code == LV_EVENT_READY || code == LV_EVENT_CANCEL) && (screen->keyboard))
    {
        screen->lock();
        lv_obj_remove_event_cb(screen->keyboard, keyboardEventCallback);
        lv_obj_del(screen->keyboard);
        screen->keyboard = nullptr;
        screen->unlock();
    }
}

void WifiScreen::textareaEventCallback(lv_event_t* e)
{
    lv_event_code_t code           = lv_event_get_code(e);
    WifiScreen*     screen         = &WifiScreen::instance();
    bool            createKeyboard = true;
    screen->lock();
    if (lv_event_get_current_target(e) == screen->ui->locationConfigurationBlock.getCityTextarea())
    {
        createKeyboard = screen->ui->locationConfigurationBlock.textareaCallback(e);
    }
    if (((code == LV_EVENT_CLICKED) || (code == LV_EVENT_FOCUSED)) && (createKeyboard))
    {
        if (!screen->keyboard)
        {
            screen->keyboard = lv_keyboard_create(screen->screen);
            lv_obj_set_size(screen->keyboard, LV_HOR_RES, LV_VER_RES / 2);
            lv_obj_add_event_cb(screen->keyboard, keyboardEventCallback, LV_EVENT_ALL, nullptr);

            if (lv_event_get_current_target(e) ==
                screen->ui->locationConfigurationBlock.getCityTextarea())
            {
                lv_timer_create(
                    [](lv_timer_t* t) {
                        WifiScreen* screen = static_cast<WifiScreen*>(t->user_data);
                        lv_obj_add_state(screen->ui->locationConfigurationBlock.getCityTextarea(),
                                         LV_STATE_FOCUSED);
                        // lv_event_send(screen->locationConfigurationBlock.getCityTextarea(),
                        // LV_EVENT_FOCUSED, NULL);
                        // lv_textarea_set_cursor_pos(screen->locationConfigurationBlock.getCityTextarea(),
                        // LV_TEXTAREA_CURSOR_LAST);
                        lv_timer_del(t);
                    },
                    0, screen);
            }
        }

        lv_keyboard_set_textarea(screen->keyboard, lv_event_get_current_target(e));
    }
    screen->unlock();
}