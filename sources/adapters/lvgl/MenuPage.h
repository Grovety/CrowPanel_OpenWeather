#pragma once

#include "Menu.h"

class MenuPage : public LvglObject
{
private:
    Menu*     menu    = nullptr;
    lv_obj_t* page    = nullptr;
    lv_obj_t* section = nullptr;

public:
    MenuPage() = default;

    bool create(Menu& m, const char* title = nullptr)
    {
        menu = &m;

        page = menu->addPage(title);
        if (!page)
            return false;
        section = lv_menu_section_create(page);

        lv_obj_t* sep = lv_menu_separator_create(section);
        lv_obj_set_style_bg_color(sep, lv_color_white(), 0);
        lv_obj_set_style_bg_opa(sep, LV_OPA_100, 0);
        lv_obj_set_height(sep, 1);

        return section != nullptr;
    }

    void remove()
    {
        if (page)
        {
            lv_obj_del(page);
            page    = nullptr;
            section = nullptr;
            menu    = nullptr;
        }
    }

    lv_obj_t* createSidebarItem(const char* label, const char* icon = nullptr)
    {
        if (!menu || !page)
            return nullptr;

        lv_obj_t* item = lv_menu_cont_create(menu->get());
        lv_obj_set_parent(item, menu->getRootPage());

        lv_obj_add_flag(item, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_layout(item, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(item, LV_FLEX_FLOW_ROW);
        lv_obj_set_style_pad_row(item, 4, 0);
        lv_obj_set_style_pad_column(item, 8, 0);
        lv_obj_set_style_pad_all(item, 8, 0);

        if (icon)
        {
            lv_obj_t* img = lv_img_create(item);
            lv_img_set_src(img, icon);
        }

        if (label)
        {
            lv_obj_t* text = lv_label_create(item);
            lv_label_set_text(text, label);
            lv_label_set_long_mode(text, LV_LABEL_LONG_SCROLL_CIRCULAR);
            lv_obj_set_flex_grow(text, 1);
        }

        lv_menu_set_load_page_event(menu->get(), item, page);

        return item;
    }

    lv_obj_t* getPage() const
    {
        return page;
    }
    lv_obj_t* getSection() const
    {
        return section;
    }
};
