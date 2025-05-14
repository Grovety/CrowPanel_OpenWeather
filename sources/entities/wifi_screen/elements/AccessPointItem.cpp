#include "AccessPointItem.h"
#ifdef COMMON_DEMO_APP
#include "entities/ui/wifi_screen/WifiScreen.h"
#else
#include "entities/wifi_screen/WifiScreen.h"
#endif

LV_IMG_DECLARE(wifi_100);
LV_IMG_DECLARE(wifi_75);
LV_IMG_DECLARE(wifi_50);
LV_IMG_DECLARE(wifi_25);
LV_IMG_DECLARE(settings);

void AccessPointItem::show(lv_obj_t* parent)
{
    // connect button
    button.create(parent, this);
    button.setEventCallback(WifiScreen::instance().wifiConnectionBlock->connectButtonCallback);
    lv_obj_set_size(button.get(), LV_PCT(100), LV_SIZE_CONTENT);
    buttonContainer.create(button.get(), LV_PCT(100), LV_SIZE_CONTENT, LV_FLEX_FLOW_COLUMN);
    buttonContainer.align(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    {
        itemContainer.create(buttonContainer.get(), LV_PCT(100), LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW);
        itemContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
        itemContainer.paddingGap(10);
        itemContainer.padding(20, 0);
        {
            // wifi icon
            icon.create(itemContainer.get());
            setIcon(rssi);
            // // wifi label
            label.create(itemContainer.get(), &lv_font_montserrat_14);
            label.align(LV_ALIGN_TOP_LEFT);
            label.setText(ssid);
            lv_obj_clear_flag(buttonContainer.get(), LV_OBJ_FLAG_CLICKABLE);
            lv_obj_clear_flag(itemContainer.get(), LV_OBJ_FLAG_CLICKABLE);
            lv_obj_clear_flag(icon.get(), LV_OBJ_FLAG_CLICKABLE);
            lv_obj_clear_flag(label.get(), LV_OBJ_FLAG_CLICKABLE);
        }
    }
    // data container
    dataContainer.create(buttonContainer.get(), LV_PCT(100), LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW);
    dataContainer.align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    dataContainer.paddingGap(20);
}

void AccessPointItem::select()
{
    lv_obj_set_style_bg_opa(buttonContainer.get(), LV_OPA_100, LV_PART_MAIN);
    lv_obj_set_style_bg_color(buttonContainer.get(), lv_color_hex(0x777777), LV_PART_MAIN);
    actionLabel.create(itemContainer.get(), &lv_font_montserrat_14);
    actionLabel.setText("\tconnecting...");
}

void AccessPointItem::deselect()
{
    lv_obj_set_style_bg_opa(buttonContainer.get(), LV_OPA_TRANSP, LV_PART_MAIN);
    actionLabel.remove();
}

void AccessPointItem::setIcon(int8_t rssi)
{
    if (rssi >= -50)
        icon.set(&wifi_100);
    else if (rssi >= -60)
        icon.set(&wifi_75);
    else if (rssi >= -70)
        icon.set(&wifi_50);
    else
        icon.set(&wifi_25);
}

void AccessPointItem::remove()
{
    icon.remove();
    label.remove();
    actionLabel.remove();
    itemContainer.remove();
    dataContainer.remove();
    buttonContainer.remove();
    button.remove();
}