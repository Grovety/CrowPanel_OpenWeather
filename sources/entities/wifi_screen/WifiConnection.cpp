#include "WifiConnection.h"
#include "WifiScreen.h"

LV_IMG_DECLARE(wifi_100);
LV_IMG_DECLARE(wifi_75);
LV_IMG_DECLARE(wifi_50);
LV_IMG_DECLARE(wifi_25);
LV_IMG_DECLARE(settings);

void WifiConnection::create(Menu& menu, lv_obj_t** _keyboard)
{
    page.create(menu, "WIFI connection");
    page.createSidebarItem("WIFI");

    /// Spacer.
    spacer.create(page.getPage(), LV_PCT(100), 20, LV_FLEX_FLOW_ROW);
    {
        connectedListContainer.create(page.getPage(), LV_PCT(80), LV_SIZE_CONTENT,
                                      LV_FLEX_FLOW_COLUMN);
        connectedListContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

        availableListContainer.create(page.getPage(), LV_PCT(80), LV_SIZE_CONTENT,
                                      LV_FLEX_FLOW_COLUMN);
        availableListContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        {
            availableLabel.create(availableListContainer.get(), &lv_font_montserrat_14);
            availableLabel.align(LV_ALIGN_TOP_LEFT);
            availableLabel.setText("Available:");
        }
    }
    keyboardSpacer.create(page.getPage(), LV_PCT(100), lv_disp_get_ver_res(NULL) / 2,
                          LV_FLEX_FLOW_COLUMN);
    keyboard = _keyboard;

    WIFI::instance().addCallback(wifiEventHandler, this);
}

void WifiConnection::wifiEventHandler(WIFI::Event event, void* context)
{
    WifiConnection* screen = (WifiConnection*)context;
    lvgl_port_lock();
    switch (event)
    {
    case WIFI::Event::CONNECTED:
        if (screen->currentAP)
            screen->currentAP->remove();
        ESP_LOGI(Tag, "wifi connected, AP removed");
        break;
    case WIFI::Event::DISCONNECTED:
        if (screen->currentAP)
            screen->currentAP->show(screen->availableListContainer.get());
        break;
    case WIFI::Event::CONNECT_FAIL:
        if (screen->currentAP)
            screen->currentAP->deselect();
        break;
    default:
        break;
    }
    ESP_LOGI(Tag, "wifi event - %d", event);
    char   ssid[MAX_SSID_LEN + 1] = { 0 };
    int8_t rssi                   = 0;
    WIFI::instance().getCurrentAP(ssid, &rssi);
    screen->updateCurrentSSID(ssid, rssi);
    lvgl_port_unlock();
}

void WifiConnection::passwordKeyboardEventHandler()
{
    WifiConnection* screen                       = WifiScreen::instance().wifiConnectionBlock;
    char            password[MAX_PASSPHRASE_LEN] = { 0 };
    strncpy(password, lv_textarea_get_text(screen->textAreaPassword), MAX_PASSPHRASE_LEN);
    if (screen->textAreaPassword)
    {
        lv_obj_remove_event_cb(screen->textAreaPassword,
                               WifiScreen::instance().textareaEventCallback);
        lv_obj_del(screen->textAreaPassword);
        screen->textAreaPassword = nullptr;
    }
    bool auto_connect = lv_obj_get_state(screen->checkboxAutoConnect) & LV_STATE_CHECKED;
    if (screen->checkboxAutoConnect)
    {
        lv_obj_del(screen->checkboxAutoConnect);
        screen->checkboxAutoConnect = nullptr;
    }

    screen->connect(screen->currentAP, password, auto_connect);
}

void WifiConnection::updateCurrentSSID(char* ssid, int8_t rssi)
{
    static bool connected = false;

    if ((!ssid) || (ssid[0] == '\0'))
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
        connectedLabel.create(connectedListContainer.get(), &lv_font_montserrat_14);
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
            currentWifiLabel.create(currentInfoContainer.get(), &lv_font_montserrat_14);
            currentWifiLabel.align(LV_ALIGN_TOP_LEFT);
            // disconnect button
            currentDisconnectContainer.create(currentItemContainer.get(), LV_PCT(50),
                                              LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW);
            currentDisconnectContainer.align(LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER,
                                             LV_FLEX_ALIGN_CENTER);

            disconnect.create(currentDisconnectContainer.get(), this);
            lv_obj_set_style_pad_all(disconnect.get(), 20, LV_PART_MAIN);
            lv_obj_set_style_border_width(disconnect.get(), 1, LV_PART_MAIN);
            lv_obj_set_style_border_color(disconnect.get(), lv_color_white(), LV_PART_MAIN);
            disconnect.setEventCallback(disconnectButtonCallback);
            lv_obj_align(disconnect.get(), LV_ALIGN_RIGHT_MID, 0, 0);
            disconnectLabel.create(disconnect.get(), &lv_font_montserrat_14);
            disconnectLabel.setText("Disconnect");
        }
    }

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

void WifiConnection::disconnectButtonCallback(lv_event_t* e, void* context)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        lvgl_port_lock();
        WIFI::instance().disconnect();
        char ssid = '\0';
        ESP_LOGI(Tag, "Disconnecting");
        (static_cast<WifiConnection*>(context))->updateCurrentSSID(&ssid, 0);
        lvgl_port_unlock();
    }
}

void WifiConnection::connectButtonCallback(lv_event_t* e, void* _context)
{
    WifiScreen*     screen = &WifiScreen::instance();
    WifiConnection* block  = WifiScreen::instance().wifiConnectionBlock;
    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
        return;

    if (!_context)
        return;

    AccessPointItem* net = (AccessPointItem*)_context;

    if (WIFI::instance().isConnected())
        return;

    block->currentAP = net;

    char password[MAX_PASSPHRASE_LEN];
    bool auto_connect;
    if (WIFI::instance().getAP(net->ssid, net->bssid, password, &auto_connect))
    {
        lvgl_port_lock();
        ESP_LOGI(Tag, "connectButton AP found lock");
        block->connect(net, password, auto_connect);
        lvgl_port_unlock();
        return;
    }

    lvgl_port_lock();
    if (block->textAreaPassword)
    {
        lv_obj_remove_event_cb(block->textAreaPassword, screen->textareaEventCallback);
        lv_obj_del(block->textAreaPassword);
        block->textAreaPassword = nullptr;
    }
    block->textAreaPassword = lv_textarea_create(net->dataContainer.get());
    lv_textarea_set_text(block->textAreaPassword, "");
    lv_textarea_set_password_mode(block->textAreaPassword, true);
    lv_textarea_set_one_line(block->textAreaPassword, true);
    lv_obj_set_width(block->textAreaPassword, lv_pct(60));
    lv_obj_add_event_cb(block->textAreaPassword, screen->textareaEventCallback, LV_EVENT_ALL, NULL);

    if (block->checkboxAutoConnect)
    {
        lv_obj_del(block->checkboxAutoConnect);
        block->checkboxAutoConnect = nullptr;
    }
    block->checkboxAutoConnect = lv_checkbox_create(net->dataContainer.get());
    lv_checkbox_set_text(block->checkboxAutoConnect, "Auto Connect");
    lv_obj_add_state(block->checkboxAutoConnect, LV_STATE_CHECKED);

    if (!*block->keyboard)
    {
        // block->keyboard = lv_keyboard_create(screen->screen);
        *block->keyboard = lv_keyboard_create(lv_scr_act());
        lv_obj_set_size(*block->keyboard, LV_HOR_RES, LV_VER_RES / 2);
        lv_obj_add_event_cb(*block->keyboard, screen->keyboardEventCallback, LV_EVENT_ALL, NULL);
    }

    lv_keyboard_set_textarea(*block->keyboard, block->textAreaPassword); /*Focus it on
                                                                              one of the text areas
                                                                              to start*/

    lv_obj_t* ta         = block->textAreaPassword;
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
    {
        ESP_LOGE(Tag, "Scrollable obj not found");
    }

    lvgl_port_unlock();
}

void WifiConnection::connect(AccessPointItem* net, char* password, bool auto_connect)
{
    currentAP          = net;
    WifiScreen& screen = WifiScreen::instance();
    ESP_LOGI(Tag, "Connecting to %s (%02X:%02X:%02X:%02X:%02X:%02X), rssi: %d, pass: %s", net->ssid,
             net->bssid[0], net->bssid[1], net->bssid[2], net->bssid[3], net->bssid[4],
             net->bssid[5], net->rssi, password);
    net->select();
    if (textAreaPassword)
    {
        lv_obj_remove_event_cb(textAreaPassword, screen.textareaEventCallback);
        lv_obj_del(textAreaPassword);
        textAreaPassword = nullptr;
    }
    if (checkboxAutoConnect)
    {
        lv_obj_del(checkboxAutoConnect);
        checkboxAutoConnect = nullptr;
    }
    if (*keyboard)
    {
        lv_obj_remove_event_cb(*keyboard, screen.keyboardEventCallback);
        lv_obj_del(*keyboard);
        *keyboard = nullptr;
    }
    WIFI::instance().connectAP(net->ssid, net->bssid, password, auto_connect);
}

lv_obj_t* WifiConnection::getAvailableAPList()
{
    return availableListContainer.get();
}

lv_obj_t* WifiConnection::getPasswordTextarea()
{
    return textAreaPassword;
}