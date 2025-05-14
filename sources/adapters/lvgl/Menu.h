#pragma once

#include "LvglObject.h"
#include "lvgl.h"

class Menu : public LvglObject
{
private:
    lv_obj_t* menu     = nullptr;
    lv_obj_t* rootPage = nullptr;
    lv_obj_t* parent   = nullptr;

public:
    Menu() = default;

    bool create(lv_obj_t* parentObj)
    {
        parent = parentObj;
        lock();
        menu = lv_menu_create(parent);
        unlock();
        return menu != nullptr;
    }

    void remove()
    {
        if (menu)
        {
            lock();
            lv_obj_del(menu);
            unlock();
            menu = nullptr;
        }
    }

    lv_obj_t* get() const
    {
        return menu;
    }

    void setRootBackButtonMode(lv_menu_mode_root_back_btn_t enabled)
    {
        lock();
        lv_menu_set_mode_root_back_btn(menu, enabled);
        unlock();
    }

    void createRootPage(const char* title = nullptr)
    {
        if (rootPage)
            return;
        lock();
        rootPage          = lv_menu_page_create(menu, const_cast<char*>(title));
        lv_obj_t* section = lv_menu_section_create(rootPage);

        lv_obj_t* sep = lv_menu_separator_create(section);
        lv_obj_set_style_bg_color(sep, lv_color_white(), 0);
        lv_obj_set_style_bg_opa(sep, LV_OPA_100, 0);
        lv_obj_set_height(sep, 1);
        unlock();
        return;
    }

    lv_obj_t* getRootPage()
    {
        return rootPage;
    }

    lv_obj_t* addPage(const char* title = nullptr)
    {
        lock();
        lv_obj_t* page = lv_menu_page_create(menu, const_cast<char*>(title));
        unlock();
        return page;
    }

    lv_obj_t* addContainer(lv_obj_t* page)
    {
        lock();
        lv_obj_t* container = lv_menu_cont_create(menu);
        lv_obj_set_layout(container, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
        lv_obj_set_style_pad_row(container, 4, 0);
        lv_obj_set_style_pad_column(container, 8, 0);
        lv_obj_set_style_pad_all(container, 8, 0);
        unlock();
        return container;
    }

    void setCurrentPage(lv_obj_t* page)
    {
        lock();
        lv_menu_set_page(menu, page);
        unlock();
    }

    void setSidebarPage(lv_obj_t* page)
    {
        lock();
        lv_menu_set_sidebar_page(menu, page);
        unlock();
    }

    void setSize(lv_coord_t width = LV_PCT(100), lv_coord_t height = LV_SIZE_CONTENT)
    {
        lock();
        lv_obj_set_size(menu, width, height);
        unlock();
    }

    void setBackgroundDarken(lv_opa_t opa)
    {
        lock();
        lv_color_t bg       = lv_obj_get_style_bg_color(menu, 0);
        lv_color_t adjusted = lv_color_darken(bg, opa);
        lv_obj_set_style_bg_color(menu, adjusted, 0);
        unlock();
    }
    void setTransparentBackground()
    {
        lock();
        lv_obj_set_style_bg_opa(menu, LV_OPA_TRANSP, 0);
        unlock();
    }
};
