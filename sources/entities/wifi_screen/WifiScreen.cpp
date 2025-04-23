#include "WifiScreen.h"
#include "entities/Brightness.h"
#include "entities/Location.h"

LV_IMG_DECLARE(wifi_100);
LV_IMG_DECLARE(wifi_75);
LV_IMG_DECLARE(wifi_50);
LV_IMG_DECLARE(wifi_25);
LV_IMG_DECLARE(settings);

void WifiScreen::createUnitsBlock()
{
    /*********************
     * UNITS CONFIGURATION
     */

    unitsConfiguration.create(contentContainer.get(), LV_PCT(100), LV_SIZE_CONTENT,
                              LV_FLEX_FLOW_COLUMN);
    unitsConfiguration.getContainer().align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                                            LV_FLEX_ALIGN_CENTER);
    {
        /// Units block: header container.
        lv_obj_set_size(unitsConfiguration.getHeader(), LV_PCT(100), LV_SIZE_CONTENT);
        lv_obj_set_align(unitsConfiguration.getHeader(), LV_ALIGN_CENTER);
        unitsHeaderContainer.create(unitsConfiguration.getHeader(), LV_PCT(100), LV_SIZE_CONTENT,
                                    LV_FLEX_FLOW_ROW);
        unitsHeaderContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
        unitsHeaderContainer.padding(20, 20);
        lv_obj_clear_flag(unitsHeaderContainer.get(), LV_OBJ_FLAG_CLICKABLE);

        lv_obj_set_style_bg_color(unitsHeaderContainer.get(), lv_color_hex(0x666666), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(unitsHeaderContainer.get(), LV_OPA_COVER, LV_PART_MAIN);

        /// Units block: header label.
        unitsHeaderLabel.create(unitsHeaderContainer.get(), &lv_font_montserrat_20);
        unitsHeaderLabel.setText(unitsHeaderString);
        unitsConfiguration.getContent().align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                                              LV_FLEX_ALIGN_CENTER);
        unitsConfiguration.getContent().padding(20, 0);
        unitsConfiguration.getContent().paddingGap(20);

        /// Temperature units.
        {
            temperatureUnitsEntryContainer.create(unitsConfiguration.getContent().get(), LV_PCT(80),
                                                  LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW);
            temperatureUnitsEntryContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                                                 LV_FLEX_ALIGN_CENTER);
            temperatureUnitsEntryContainer.paddingGap(20);
            temperatureUnitsEntryLabel.create(temperatureUnitsEntryContainer.get(),
                                              &lv_font_montserrat_20, LV_ALIGN_LEFT_MID);
            temperatureUnitsEntryLabel.setText("Temperature units:");

            temperatureDropDown = lv_dropdown_create(temperatureUnitsEntryContainer.get());
            lv_dropdown_set_options(temperatureDropDown, Units::getTemperatureString4Lvgl());
            lv_obj_add_event_cb(temperatureDropDown, unitsConfigurationHandler,
                                LV_EVENT_VALUE_CHANGED, NULL);
        }

        /// Pressure.
        {
            pressureUnitsEntryContainer.create(unitsConfiguration.getContent().get(), LV_PCT(80),
                                               LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW);
            pressureUnitsEntryContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                                              LV_FLEX_ALIGN_CENTER);
            pressureUnitsEntryContainer.paddingGap(20);
            pressureUnitsEntryLabel.create(pressureUnitsEntryContainer.get(),
                                           &lv_font_montserrat_20, LV_ALIGN_LEFT_MID);
            pressureUnitsEntryLabel.setText("Pressure units:");

            pressureDropDown = lv_dropdown_create(pressureUnitsEntryContainer.get());
            lv_dropdown_set_options(pressureDropDown, Units::getPressureString4Lvgl());
            lv_obj_add_event_cb(pressureDropDown, unitsConfigurationHandler, LV_EVENT_VALUE_CHANGED,
                                NULL);
        }
    }
}

void WifiScreen::unitsConfigurationHandler(lv_event_t* e)
{
    WifiScreen* screen = &WifiScreen::instance();
    screen->lock();
    if (lv_event_get_target(e) == screen->temperatureDropDown)
    {
        Units::instance().setTemperature(
            static_cast<Units::Temperature>(lv_dropdown_get_selected(screen->temperatureDropDown)));
        ESP_LOGI(Tag, "Temperature units changed to %s", Units::instance().getTemperatureString());
    } else if (lv_event_get_target(e) == screen->pressureDropDown)
    {
        Units::instance().setPressure(
            static_cast<Units::Pressure>(lv_dropdown_get_selected(screen->pressureDropDown)));
        ESP_LOGI(Tag, "Pressure units changed to %s", Units::instance().getPressureString());
    }
    screen->unlock();
}

void WifiScreen::create(lv_obj_t* screen_)
{
    createScreen();
    mainContainer.create(screen, LV_PCT(100), LV_PCT(100), LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_bg_color(mainContainer.get(), lv_color_hex(0x222222), LV_PART_MAIN);
    mainContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    {
        /// Screen header.
        header.create(mainContainer.get());
        // if wifi get current == true

        contentContainer.create(mainContainer.get(), LV_PCT(100), LV_PCT(90), LV_FLEX_FLOW_COLUMN);
        contentContainer.padding(20, 60);
        contentContainer.paddingGap(10);
        contentContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

        /*********************
         * UNITS CONFIGURATION
         */
        createUnitsBlock();

        /*********************
         * BRIGHTNESS CONFIGURATION
         */

        brightnessConfiguration.create(contentContainer.get(), LV_PCT(100), LV_SIZE_CONTENT,
                                       LV_FLEX_FLOW_COLUMN);
        brightnessConfiguration.getContainer().align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                                                     LV_FLEX_ALIGN_CENTER);
        {
            /// City configuration block: header container.
            lv_obj_set_size(brightnessConfiguration.getHeader(), LV_PCT(100), LV_SIZE_CONTENT);
            lv_obj_set_align(brightnessConfiguration.getHeader(), LV_ALIGN_CENTER);
            brightnessHeaderContainer.create(brightnessConfiguration.getHeader(), LV_PCT(100),
                                             LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW);
            brightnessHeaderContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START,
                                            LV_FLEX_ALIGN_CENTER);
            brightnessHeaderContainer.padding(20, 20);
            lv_obj_clear_flag(brightnessHeaderContainer.get(), LV_OBJ_FLAG_CLICKABLE);

            lv_obj_set_style_bg_color(brightnessHeaderContainer.get(), lv_color_hex(0x666666),
                                      LV_PART_MAIN);
            lv_obj_set_style_bg_opa(brightnessHeaderContainer.get(), LV_OPA_COVER, LV_PART_MAIN);

            /// City configuration block: header label.
            brightnessHeaderLabel.create(brightnessHeaderContainer.get(), &lv_font_montserrat_20);
            brightnessHeaderLabel.setText(brighnessHeaderString);
            brightnessConfiguration.getContent().align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                                                       LV_FLEX_ALIGN_CENTER);
            brightnessConfiguration.getContent().padding(20, 0);
            brightnessConfiguration.getContent().paddingGap(20);

            /// Brightness switch.
            {
                brightnessSwitchContainer.create(brightnessConfiguration.getContent().get(),
                                                 LV_PCT(80), LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW);
                brightnessSwitchContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                                                LV_FLEX_ALIGN_CENTER);
                brightnessSwitchContainer.paddingGap(20);
                brightnessSwitchLabel.create(brightnessSwitchContainer.get(),
                                             &lv_font_montserrat_20, LV_ALIGN_LEFT_MID);
                brightnessSwitchLabel.setText("Auto:");

                brightnessSwitch = lv_switch_create(brightnessSwitchContainer.get());
                lv_obj_add_event_cb(brightnessSwitch, brightnessSwitchCallback,
                                    LV_EVENT_VALUE_CHANGED, NULL);
                /// get & set default.
                lv_obj_add_state(brightnessSwitch, LV_STATE_CHECKED);
            }

            /// Brightness slider.
            {
                brightnessSliderContainer.create(brightnessConfiguration.getContent().get(),
                                                 LV_PCT(80), LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW);
                brightnessSliderContainer.paddingGap(20);
                brightnessSliderContainer.padding(10, 0);
                brightnessSliderContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                                                LV_FLEX_ALIGN_CENTER);
                brightnessSliderContainer.paddingGap(20);
                brightnessSliderLabel.create(brightnessSliderContainer.get(),
                                             &lv_font_montserrat_20, LV_ALIGN_LEFT_MID);
                brightnessSliderLabel.setText("Manual:");
                brightnessSlider = lv_slider_create(brightnessSliderContainer.get());
                lv_obj_add_event_cb(brightnessSlider, brightnessSliderCallback,
                                    LV_EVENT_VALUE_CHANGED, NULL);
                lv_slider_set_value(brightnessSlider, 17, LV_ANIM_OFF);

                brightnessManualValueLabel.create(brightnessSliderContainer.get(),
                                                  &lv_font_montserrat_20, LV_ALIGN_RIGHT_MID);

                lv_style_init(&brightnessSliderStyle);
                lv_style_set_opa(&brightnessSliderStyle, LV_OPA_50);
                lv_style_set_bg_color(&brightnessSliderStyle, lv_color_hex(0xAAAAAA));
                lv_style_set_border_color(&brightnessSliderStyle, lv_color_hex(0xCCCCCC));

                /// get default.
                /// set default.
                brightnessManualValueLabel.setText("17%");
                setBirghtnessSliderDisabled(lv_obj_has_state(brightnessSwitch, LV_STATE_CHECKED));
            }
        }

        /********************
         * CITY CONFIGURATION
         */
        cityConfiguration.create(contentContainer.get(), LV_PCT(100), LV_SIZE_CONTENT,
                                 LV_FLEX_FLOW_ROW);
        cityConfiguration.getContainer().align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                                               LV_FLEX_ALIGN_CENTER);
        {
            /// City configuration block: header container.
            lv_obj_set_size(cityConfiguration.getHeader(), LV_PCT(100), LV_SIZE_CONTENT);
            lv_obj_set_align(cityConfiguration.getHeader(), LV_ALIGN_CENTER);
            cityHeaderContainer.create(cityConfiguration.getHeader(), LV_PCT(100), LV_SIZE_CONTENT,
                                       LV_FLEX_FLOW_ROW);
            cityHeaderContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START,
                                      LV_FLEX_ALIGN_CENTER);
            cityHeaderContainer.padding(20, 20);
            lv_obj_clear_flag(cityHeaderContainer.get(), LV_OBJ_FLAG_CLICKABLE);

            lv_obj_set_style_bg_color(cityHeaderContainer.get(), lv_color_hex(0x666666),
                                      LV_PART_MAIN);
            lv_obj_set_style_bg_opa(cityHeaderContainer.get(), LV_OPA_COVER, LV_PART_MAIN);

            /// City configuration block: header label.
            cityHeaderLabel.create(cityHeaderContainer.get(), &lv_font_montserrat_20);
            cityHeaderLabel.setText(cityHeaderString);
            cityConfiguration.getContent().align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                                                 LV_FLEX_ALIGN_CENTER);
            cityConfiguration.getContent().padding(20, 0);
            {
                cityContentWrapContainer.create(cityConfiguration.getContent().get(), LV_PCT(80),
                                                LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW);
                cityContentWrapContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                                               LV_FLEX_ALIGN_CENTER);
                cityContentWrapContainer.paddingGap(20);
                cityEntryLabel.create(cityContentWrapContainer.get(), &lv_font_montserrat_20,
                                      LV_ALIGN_LEFT_MID);
                cityEntryLabel.setText("City:");
                textAreaCity = lv_textarea_create(cityContentWrapContainer.get());
                Location::instance().get(cityText);
                if (cityText[0] == '\0')
                    snprintf(cityText, sizeof(cityText), "Auto detect");
                lv_textarea_set_text(textAreaCity, cityText);
                lv_textarea_set_one_line(textAreaCity, true);
                lv_obj_set_width(textAreaCity, lv_pct(60));
                lv_obj_add_event_cb(textAreaCity, textareaEventCallback, LV_EVENT_ALL, NULL);
                lv_style_init(&cityTextareaStyle);
                lv_style_set_text_color(&cityTextareaStyle, lv_color_hex(0x777777));
                lv_obj_add_style(textAreaCity, &cityTextareaStyle, LV_PART_MAIN);
            }
        }

        /******************
         * WIFI CONFIGURATION
         */
        wifiConfiguration.create(contentContainer.get(), LV_PCT(100), LV_SIZE_CONTENT,
                                 LV_FLEX_FLOW_COLUMN);
        wifiConfiguration.getContainer().align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                                               LV_FLEX_ALIGN_CENTER);
        {
            /// Wifi configuration block: header container.
            lv_obj_set_size(wifiConfiguration.getHeader(), LV_PCT(100), LV_SIZE_CONTENT);
            lv_obj_set_align(wifiConfiguration.getHeader(), LV_ALIGN_CENTER);
            wifiHeaderContainer.create(wifiConfiguration.getHeader(), LV_PCT(100), LV_SIZE_CONTENT,
                                       LV_FLEX_FLOW_ROW);
            wifiHeaderContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START,
                                      LV_FLEX_ALIGN_CENTER);
            wifiHeaderContainer.padding(20, 20);
            lv_obj_clear_flag(wifiHeaderContainer.get(), LV_OBJ_FLAG_CLICKABLE);

            lv_obj_set_style_bg_color(wifiHeaderContainer.get(), lv_color_hex(0x666666),
                                      LV_PART_MAIN);
            lv_obj_set_style_bg_opa(wifiHeaderContainer.get(), LV_OPA_COVER, LV_PART_MAIN);

            /// Wifi configuration block: header label.
            wifiHeaderLabel.create(wifiHeaderContainer.get(), &lv_font_montserrat_20);
            wifiHeaderLabel.setText(WifiHeaderString);

            /// /// Wifi configuration block: content.
            wifiConfiguration.getContent().align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                                                 LV_FLEX_ALIGN_CENTER);
            wifiConfiguration.getContent().paddingGap(20);
            wifiConfiguration.getContent().padding(20, 0);
            {
                connectedListContainer.create(wifiConfiguration.getContent().get(), LV_PCT(80),
                                              LV_SIZE_CONTENT, LV_FLEX_FLOW_COLUMN);
                connectedListContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START,
                                             LV_FLEX_ALIGN_START);
                // get AP list
                // availableListContainer
                {
                    availableListContainer.create(wifiConfiguration.getContent().get(), LV_PCT(80),
                                                  LV_SIZE_CONTENT, LV_FLEX_FLOW_COLUMN);
                    availableListContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START,
                                                 LV_FLEX_ALIGN_START);
                    {
                        // connected label
                        availableLabel.create(availableListContainer.get(), &lv_font_montserrat_20);
                        availableLabel.align(LV_ALIGN_TOP_LEFT);
                        availableLabel.setText("Available:");
                    }
                }
            }
        }
        keyboardSpacer.create(contentContainer.get(), LV_PCT(100), lv_disp_get_ver_res(NULL) / 2,
                              LV_FLEX_FLOW_COLUMN);
    }

    WIFI::instance().addCallback(wifiEventHandler, NULL);

    // start timer to update.
    updateTimer = xTimerCreate("updateTimer", 60000, pdTRUE, (void*)this, updateTimerCallback);
    xTimerStart(updateTimer, 0);
}

void WifiScreen::wifiEventHandler(WIFI::Event event, void* context)
{
    switch (event)
    {
    case WIFI::Event::CONNECTED:
        if (WifiScreen::instance().currentAP)
            WifiScreen::instance().currentAP->remove();
        ESP_LOGI(Tag, "wifi connected, AP removed");
        break;
    case WIFI::Event::DISCONNECTED:
        if (WifiScreen::instance().currentAP)
            WifiScreen::instance().currentAP->show(
                WifiScreen::instance().availableListContainer.get());
        break;
    case WIFI::Event::CONNECT_FAIL:
        if (WifiScreen::instance().currentAP)
            WifiScreen::instance().currentAP->deselect();
        break;
    default:
        break;
    }
    ESP_LOGI(Tag, "wifi event - %d", event);
    char   ssid[MAX_SSID_LEN + 1] = { 0 };
    int8_t rssi                   = 0;
    WIFI::instance().getCurrentAP(ssid, &rssi);
    WifiScreen::instance().updateCurrentSSID(ssid, rssi);
}

void WifiScreen::setBirghtnessSliderDisabled(bool disabled)
{
    if (disabled)
    {
        lv_obj_add_state(brightnessSlider, LV_STATE_DISABLED);
        lv_obj_clear_flag(brightnessSlider, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_style(brightnessSlider, &brightnessSliderStyle,
                         LV_PART_MAIN | LV_STATE_DISABLED);
    } else
    {
        lv_obj_clear_state(brightnessSlider, LV_STATE_DISABLED);
        lv_obj_add_flag(brightnessSlider, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_remove_style(brightnessSlider, &brightnessSliderStyle,
                            LV_PART_MAIN | LV_STATE_DISABLED);
    }
}

void WifiScreen::connect(AccessPointItem* net, char* password, bool autoconnect)
{
    ESP_LOGI(Tag, "Connecting to %s (%02X:%02X:%02X:%02X:%02X:%02X), rssi: %d, pass: %s", net->ssid,
             net->bssid[0], net->bssid[1], net->bssid[2], net->bssid[3], net->bssid[4],
             net->bssid[5], net->rssi, password);
    net->select();
    if (textAreaPassword)
    {
        lv_obj_remove_event_cb(textAreaPassword, textareaEventCallback);
        lv_obj_del(textAreaPassword);
        textAreaPassword = nullptr;
    }
    if (checkboxAutoconnect)
    {
        lv_obj_del(checkboxAutoconnect);
        checkboxAutoconnect = nullptr;
    }
    if (keyboard)
    {
        lv_obj_remove_event_cb(keyboard, keyboardEventCallback);
        lv_obj_del(keyboard);
        keyboard = nullptr;
    }
    WIFI::instance().connectAP(net->ssid, net->bssid, password, autoconnect);
}

void WifiScreen::updateTimerCallback(TimerHandle_t xTimer)
{
    WifiScreen* screen             = (WifiScreen*)pvTimerGetTimerID(xTimer);
    char        ssid[MAX_SSID_LEN] = { 0 };
    int8_t      rssi               = 0;

    screen->lock();
    if (WIFI::instance().isConnected())
    {
        WIFI::instance().getCurrentAP(ssid, &rssi);
        screen->updateCurrentSSID(ssid, rssi);
    }
    screen->updateWIFIList();
    screen->unlock();
}

void WifiScreen::updateWIFIList()
{
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
        for (auto& net : wifiList)
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
            AccessPointItem* net = new AccessPointItem(ssid, rssi, ap_records[i].bssid);
            wifiList.emplace_back(net);
            net->show(WifiScreen::instance().availableListContainer.get());
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
        ESP_LOGI(Tag,
                 "AP (auto) SSID: %s (BSSID: %02x:%02x:%02x:%02x:%02x:%02x), auto - %d, PASS: %s",
                 ap.ssid, ap.bssid[0], ap.bssid[1], ap.bssid[2], ap.bssid[3], ap.bssid[4],
                 ap.bssid[5], ap.autoconnect, ap.pass);

    wifiList.erase(std::remove_if(wifiList.begin(), wifiList.end(),
                                  [&](AccessPointItem* net) {
                                      bool found = std::find(found_ssids.begin(), found_ssids.end(),
                                                             net->ssid) != found_ssids.end();
                                      if (!found)
                                      {
                                          lvgl_port_lock();
                                          net->remove();
                                          delete net;
                                          lvgl_port_unlock();
                                          return true;
                                      }
                                      return false;
                                  }),
                   wifiList.end());

    if (bestAutoconnect && !WIFI::instance().isConnected())
    {
        ESP_LOGI(Tag, "Autoconnect to best SSID: %s (rssi: %d)", bestAutoconnect->ssid, bestRSSI);
        AccessPointItem* netToConnect = nullptr;
        for (auto& net : wifiList)
        {
            if (memcmp(net->bssid, bestAutoconnect->bssid, 6) == 0)
            {
                netToConnect = net;
                break;
            }
        }
        if (netToConnect)
        {
            currentAP = netToConnect;
            connect(netToConnect, bestAutoconnect->pass, bestAutoconnect->autoconnect);
        }
    }
}

void WifiScreen::setBrightness(bool autoUpdate, uint8_t level)
{
    if (autoUpdate)
    {
        lv_obj_add_state(brightnessSwitch, LV_STATE_CHECKED);
    } else
        lv_obj_clear_state(brightnessSwitch, LV_STATE_CHECKED);

    setBirghtnessSliderDisabled(autoUpdate);

    lv_slider_set_value(brightnessSlider, level, LV_ANIM_OFF);
    char textLevel[10] = { 0 };
    snprintf(textLevel, 10, "%d%%", level);
    brightnessManualValueLabel.setText(textLevel);
}

void WifiScreen::updateCurrentSSID(char* ssid, int8_t rssi)
{
    static bool connected = false;

    if (ssid[0] == '\0')
    {
        connectedLabel.remove();
        spaceContainer.remove();
        currentWifiIcon.remove();
        currentWifiLabel.remove();
        currentInfoContainer.remove();
        disconnectLabel.remove();
        disconnect.remove();
        currentDisconnectContainer.remove();
        currentItemContainer.remove();
        connected = false;
        return;
    }

    if (!connected)
    {
        connected = true;

        // connected label
        connectedLabel.create(connectedListContainer.get(), &lv_font_montserrat_20);
        connectedLabel.align(LV_ALIGN_TOP_LEFT);
        connectedLabel.setText("Connected:");
        spaceContainer.create(connectedListContainer.get(), LV_PCT(100), 10, LV_FLEX_FLOW_ROW);
        // wifiAPInfoContainer (row)
        currentItemContainer.create(connectedListContainer.get(), LV_PCT(100), LV_SIZE_CONTENT,
                                    LV_FLEX_FLOW_ROW);
        currentItemContainer.align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
        currentItemContainer.padding(10, 0);
        {
            currentInfoContainer.create(currentItemContainer.get(), LV_PCT(50), LV_SIZE_CONTENT,
                                        LV_FLEX_FLOW_ROW);
            currentInfoContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                                       LV_FLEX_ALIGN_CENTER);
            currentInfoContainer.paddingGap(10);
            // wifi icon
            currentWifiIcon.create(currentInfoContainer.get());

            // wifi label
            currentWifiLabel.create(currentInfoContainer.get(), &lv_font_montserrat_20);
            currentWifiLabel.align(LV_ALIGN_TOP_LEFT);
            // disconnect button
            currentDisconnectContainer.create(currentItemContainer.get(), LV_PCT(50),
                                              LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW);
            currentDisconnectContainer.align(LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER,
                                             LV_FLEX_ALIGN_CENTER);

            disconnect.create(currentDisconnectContainer.get());
            lv_obj_set_style_pad_all(disconnect.get(), 20, LV_PART_MAIN);
            lv_obj_set_style_border_width(disconnect.get(), 1, LV_PART_MAIN);
            lv_obj_set_style_border_color(disconnect.get(), lv_color_white(), LV_PART_MAIN);
            disconnect.setEventCallback(disconnectButtonCallback);
            lv_obj_align(disconnect.get(), LV_ALIGN_RIGHT_MID, 0, 0);
            disconnectLabel.create(disconnect.get(), &lv_font_montserrat_20);
            disconnectLabel.setText("Disconnect");
        }
    }

    header.updateRSSI(rssi);
    if (rssi >= -50)
        currentWifiIcon.set(&wifi_100);
    else if (rssi >= -60)
        currentWifiIcon.set(&wifi_75);
    else if (rssi >= -70)
        currentWifiIcon.set(&wifi_50);
    else
        currentWifiIcon.set(&wifi_25);
    currentWifiLabel.setText(ssid);
}

void WifiScreen::disconnectButtonCallback(lv_event_t* e, void* context)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        WifiScreen::instance().lock();
        WIFI::instance().disconnect();
        char ssid = '\0';
        ESP_LOGI(Tag, "Disconnecting");
        WifiScreen::instance().updateCurrentSSID(&ssid, 0);
        WifiScreen::instance().unlock();
    }
}

void WifiScreen::keyboardEventCallback(lv_event_t* e)
{
    lv_event_code_t code   = lv_event_get_code(e);
    WifiScreen*     screen = &WifiScreen::instance();
    if (code == LV_EVENT_READY)
    {
        screen->lock();
        if (lv_keyboard_get_textarea(screen->keyboard) == screen->textAreaPassword)
            passwordKeyboardEventHandler();
        else if (lv_keyboard_get_textarea(screen->keyboard) == screen->textAreaCity)
            cityKeyboardEventHandler();
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

void WifiScreen::passwordKeyboardEventHandler()
{
    WifiScreen* screen                       = &WifiScreen::instance();
    char        password[MAX_PASSPHRASE_LEN] = { 0 };
    strncpy(password, lv_textarea_get_text(screen->textAreaPassword), MAX_PASSPHRASE_LEN);
    if (screen->textAreaPassword)
    {
        lv_obj_remove_event_cb(screen->textAreaPassword, textareaEventCallback);
        lv_obj_del(screen->textAreaPassword);
        screen->textAreaPassword = nullptr;
    }
    bool autoconnect = lv_obj_get_state(screen->checkboxAutoconnect) & LV_STATE_CHECKED;
    if (screen->checkboxAutoconnect)
    {
        lv_obj_del(screen->checkboxAutoconnect);
        screen->checkboxAutoconnect = nullptr;
    }

    screen->connect(screen->currentAP, password, autoconnect);
}

void WifiScreen::cityKeyboardEventHandler()
{
    WifiScreen* screen                   = &WifiScreen::instance();
    char        city[MAX_PASSPHRASE_LEN] = { 0 };
    strncpy(city, lv_textarea_get_text(screen->textAreaCity), 32);
    ESP_LOGI(Tag, "City: %s", city);
    if (Weather::instance().checkLocation(city))
    {
        Location::instance().setManual(city);
        lv_timer_create(
            [](lv_timer_t* t) {
                WifiScreen* screen = (WifiScreen*)t->user_data;
                lv_obj_set_style_border_color(screen->textAreaCity, lv_color_hex(0x009900),
                                              0); // green
                lv_obj_set_style_border_width(screen->textAreaCity, 3, 0);
                lv_event_send(screen->textAreaCity, LV_EVENT_DEFOCUSED, NULL);
                lv_obj_clear_state(screen->textAreaCity, LV_STATE_FOCUSED);
                lv_obj_clear_state(screen->textAreaCity, LV_STATE_EDITED);
                lv_textarea_clear_selection(screen->textAreaCity);
                lv_timer_del(t);
            },
            0, screen);

        lv_timer_create(
            [](lv_timer_t* t) {
                WifiScreen* screen = (WifiScreen*)t->user_data;
                lv_obj_set_style_border_width(screen->textAreaCity, 0, 0);
                // In order to print in correct case (First letter upper-case, others low-case).
                if (!lv_obj_has_state(screen->textAreaCity, LV_STATE_FOCUSED))
                {
                    if (!Location::instance().get(screen->cityText))
                        snprintf(screen->cityText, Location::MaxCityNameLength, "Auto detect");
                    lv_textarea_set_text(screen->textAreaCity, screen->cityText);
                }
                lv_timer_del(t);
            },
            2000, screen);
    } else
    {
        lv_timer_create(
            [](lv_timer_t* t) {
                WifiScreen* screen = (WifiScreen*)t->user_data;

                lv_obj_set_style_border_color(screen->textAreaCity, lv_color_hex(0x990000), // red
                                              0);
                lv_obj_set_style_border_width(screen->textAreaCity, 3, 0);
                lv_event_send(screen->textAreaCity, LV_EVENT_DEFOCUSED, NULL);
                lv_obj_clear_state(screen->textAreaCity, LV_STATE_FOCUSED);
                lv_obj_clear_state(screen->textAreaCity, LV_STATE_EDITED);
                lv_textarea_clear_selection(screen->textAreaCity);
                lv_timer_del(t);
            },
            0, screen);

        lv_timer_create(
            [](lv_timer_t* t) {
                WifiScreen* screen = (WifiScreen*)t->user_data;
                if (!lv_obj_has_state(screen->textAreaCity, LV_STATE_FOCUSED))
                {
                    if (!Location::instance().get(screen->cityText))
                        snprintf(screen->cityText, Location::MaxCityNameLength, "Auto detect");
                    lv_textarea_set_text(screen->textAreaCity, screen->cityText);
                }
                lv_obj_set_style_border_width(screen->textAreaCity, 0, 0);
                lv_timer_del(t);
            },
            2000, screen);
    }
}

bool WifiScreen::cityTextareaCallback(lv_event_t* e)
{
    lv_event_code_t code      = lv_event_get_code(e);
    WifiScreen*     screen    = &WifiScreen::instance();
    bool            retVal    = true;
    static bool     defocused = true;
    if ((code == LV_EVENT_CLICKED) || (code == LV_EVENT_FOCUSED))
    {
        if (!WIFI::instance().isConnected())
        {
            lv_timer_create(
                [](lv_timer_t* t) {
                    WifiScreen* screen = (WifiScreen*)t->user_data;
                    lv_obj_set_style_border_color(screen->textAreaCity, lv_color_hex(0xaa6600),
                                                  0); // orange
                    lv_obj_set_style_border_width(screen->textAreaCity, 3, 0);
                    lv_textarea_set_text(screen->textAreaCity, "WIFI not connected");
                    lv_event_send(screen->textAreaCity, LV_EVENT_DEFOCUSED, NULL);
                    lv_obj_clear_state(screen->textAreaCity, LV_STATE_FOCUSED);
                    lv_obj_clear_state(screen->textAreaCity, LV_STATE_EDITED);
                    lv_textarea_clear_selection(screen->textAreaCity);
                    lv_timer_del(t);
                },
                0, screen);

            lv_timer_create(
                [](lv_timer_t* t) {
                    WifiScreen* screen = (WifiScreen*)t->user_data;
                    if (!Location::instance().get(screen->cityText))
                        snprintf(screen->cityText, sizeof(screen->cityText), "Auto detect");
                    lv_textarea_set_text(screen->textAreaCity, screen->cityText);
                    lv_obj_set_style_border_width(screen->textAreaCity, 0, 0);
                    lv_timer_del(t);
                    ESP_LOGI(Tag, "textAreaCity: %s", screen->cityText);
                },
                3500, screen);
            retVal = false;
        } else if (strncmp(screen->cityText, "Auto detect", 11) == 0)
        {
            lv_timer_create(
                [](lv_timer_t* t) {
                    WifiScreen* screen = (WifiScreen*)t->user_data;
                    memset(screen->cityText, '\0', Location::MaxCityNameLength);
                    lv_textarea_set_text(screen->textAreaCity, screen->cityText);
                    lv_timer_del(t);
                },
                0, screen);
        }

        if (defocused)
        {
            defocused = false;
            lv_timer_create(
                [](lv_timer_t* t) {
                    WifiScreen* screen = (WifiScreen*)t->user_data;
                    lv_style_set_text_color(&screen->cityTextareaStyle, lv_color_hex(0xffffff));
                    lv_obj_refresh_style(screen->textAreaCity, LV_PART_MAIN, LV_STYLE_PROP_ANY);
                    lv_timer_del(t);
                },
                0, screen);
        }
    } else if (code == LV_EVENT_DEFOCUSED)
    {
        if (!defocused)
        {
            defocused = true;
            lv_timer_create(
                [](lv_timer_t* t) {
                    WifiScreen* screen = (WifiScreen*)t->user_data;
                    lv_style_set_text_color(&screen->cityTextareaStyle, lv_color_hex(0x777777));
                    lv_obj_refresh_style(screen->textAreaCity, LV_PART_MAIN, LV_STYLE_PROP_ANY);
                    lv_timer_del(t);
                },
                0, screen);
            if (screen->keyboard)
            {
                lv_obj_remove_event_cb(screen->keyboard, keyboardEventCallback);
                lv_obj_del(screen->keyboard);
                screen->keyboard = nullptr;
            }
        }
        retVal = false;
    }
    return retVal;
}

void WifiScreen::textareaEventCallback(lv_event_t* e)
{
    lv_event_code_t code           = lv_event_get_code(e);
    WifiScreen*     screen         = &WifiScreen::instance();
    bool            createKeyboard = true;
    screen->lock();
    if (lv_event_get_current_target(e) == screen->textAreaCity)
    {
        createKeyboard = screen->cityTextareaCallback(e);
    }
    if (((code == LV_EVENT_CLICKED) || (code == LV_EVENT_FOCUSED)) && (createKeyboard))
    {
        if (!screen->keyboard)
        {
            screen->keyboard = lv_keyboard_create(screen->screen);
            lv_obj_set_size(screen->keyboard, LV_HOR_RES, LV_VER_RES / 2);
            lv_obj_add_event_cb(screen->keyboard, keyboardEventCallback, LV_EVENT_ALL, NULL);

            if (lv_event_get_current_target(e) == screen->textAreaCity)
            {
                lv_timer_create(
                    [](lv_timer_t* t) {
                        WifiScreen* screen = static_cast<WifiScreen*>(t->user_data);
                        lv_obj_add_state(screen->textAreaCity, LV_STATE_FOCUSED);
                        // lv_event_send(screen->textAreaCity, LV_EVENT_FOCUSED, NULL);
                        // lv_textarea_set_cursor_pos(screen->textAreaCity,
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

void WifiScreen::brightnessSwitchCallback(lv_event_t* e)
{
    WifiScreen::instance().lock();
    bool value = lv_obj_has_state(lv_event_get_current_target(e), LV_STATE_CHECKED);
    ESP_LOGI(Tag, "Auto brightness: %s", value ? "ON" : "OFF");
    WifiScreen::instance().setBirghtnessSliderDisabled(value);
    Brightness::instance().set(value, lv_slider_get_value(WifiScreen::instance().brightnessSlider));
    WifiScreen::instance().unlock();
}

void WifiScreen::brightnessSliderCallback(lv_event_t* e)
{
    WifiScreen::instance().lock();
    int value = lv_slider_get_value(lv_event_get_current_target(e));
    ESP_LOGI(Tag, "Brightness: %d", value);
    char text[10] = { 0 };
    snprintf(text, 10, "%d%%", value);
    WifiScreen::instance().brightnessManualValueLabel.setText(text);
    Brightness::instance().set(false, lv_slider_get_value(WifiScreen::instance().brightnessSlider));
    WifiScreen::instance().unlock();
}

void WifiScreen::connectButtonCallback(lv_event_t* e, void* _context)
{
    WifiScreen* screen = &WifiScreen::instance();
    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
        return;

    if (!_context)
        return;

    AccessPointItem* net = (AccessPointItem*)_context;

    if (WIFI::instance().isConnected())
        return;

    screen->currentAP = net;

    char password[MAX_PASSPHRASE_LEN];
    bool autoconnect;
    if (WIFI::instance().getAP(net->ssid, net->bssid, password, &autoconnect))
    {
        screen->lock();
        ESP_LOGI(Tag, "connectButton AP found lock");
        screen->connect(net, password, autoconnect);
        screen->unlock();
        return;
    }

    screen->lock();
    if (screen->textAreaPassword)
    {
        lv_obj_remove_event_cb(screen->textAreaPassword, screen->textareaEventCallback);
        lv_obj_del(screen->textAreaPassword);
        screen->textAreaPassword = nullptr;
    }
    screen->textAreaPassword = lv_textarea_create(net->dataContainer.get());
    lv_textarea_set_text(screen->textAreaPassword, "");
    lv_textarea_set_password_mode(screen->textAreaPassword, true);
    lv_textarea_set_one_line(screen->textAreaPassword, true);
    lv_obj_set_width(screen->textAreaPassword, lv_pct(60));
    lv_obj_add_event_cb(screen->textAreaPassword, textareaEventCallback, LV_EVENT_ALL, NULL);

    if (screen->checkboxAutoconnect)
    {
        lv_obj_del(screen->checkboxAutoconnect);
        screen->checkboxAutoconnect = nullptr;
    }
    screen->checkboxAutoconnect = lv_checkbox_create(net->dataContainer.get());
    lv_checkbox_set_text(screen->checkboxAutoconnect, "Autoconnect");
    lv_obj_add_state(screen->checkboxAutoconnect, LV_STATE_CHECKED);

    if (!screen->keyboard)
    {
        screen->keyboard = lv_keyboard_create(screen->screen);
        lv_obj_set_size(screen->keyboard, LV_HOR_RES, LV_VER_RES / 2);
        lv_obj_add_event_cb(screen->keyboard, keyboardEventCallback, LV_EVENT_ALL, NULL);
    }

    lv_keyboard_set_textarea(screen->keyboard, screen->textAreaPassword); /*Focus it on one of
                                                                             the text areas to
                                                                             start*/

    lv_obj_t* ta         = screen->textAreaPassword;
    lv_obj_t* scrollable = ta;
    while (scrollable && !lv_obj_has_flag(scrollable, LV_OBJ_FLAG_SCROLLABLE))
    {
        scrollable = lv_obj_get_parent(scrollable);
    }
    if (scrollable)
    {
        lv_area_t ta_coords;
        lv_area_t scroll_coords;
        lv_obj_get_coords(ta, &ta_coords);
        lv_obj_get_coords(scrollable, &scroll_coords);
        lv_coord_t ta_y     = ta_coords.y1 - scroll_coords.y1;
        lv_coord_t ta_h     = lv_area_get_height(&ta_coords);
        lv_coord_t scroll_h = lv_obj_get_height(scrollable);
        lv_coord_t target_y = ta_y + ta_h / 2 - scroll_h / 2;

        lv_obj_scroll_to_y(scrollable, target_y, LV_ANIM_OFF);
        ESP_LOGI(Tag, "Scrolling to %d(%d, %d, %d)", target_y, ta_y, ta_h, scroll_h);
    } else
        ESP_LOGE(Tag, "Scrollable obj not found");

    screen->unlock();
}

void WifiScreen::setUnits(Units::Temperature temperature, Units::Pressure pressure)
{
    lv_dropdown_set_selected(temperatureDropDown, static_cast<uint16_t>(temperature));
    lv_dropdown_set_selected_highlight(temperatureDropDown, true);
    lv_dropdown_set_selected(pressureDropDown, static_cast<uint16_t>(pressure));
    lv_dropdown_set_selected_highlight(pressureDropDown, true);
}