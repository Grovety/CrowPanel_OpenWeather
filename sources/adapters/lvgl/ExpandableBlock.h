#pragma once

#include "Button.h"
#include "FlexContainer.h"
#include "LvglObject.h"
#include "lvgl.h"
#include <string>

class ExpandableBlock : public LvglObject {
    FlexContainer container;
    FlexContainer content;
    Button header;
    bool expanded = false;

    void updateContentVisibility() {
        lv_async_call(
            [](void *ctx) {
                ExpandableBlock *me = static_cast<ExpandableBlock *>(ctx);
                if (me->expanded) {
                    lv_obj_clear_flag(me->content.get(), LV_OBJ_FLAG_HIDDEN);
                    lv_obj_set_height(me->content.get(), LV_SIZE_CONTENT);
                } else {
                    lv_obj_set_height(me->content.get(), 0);
                    lv_obj_add_flag(me->content.get(), LV_OBJ_FLAG_HIDDEN);
                }

                lv_obj_invalidate(me->container.get());
            },
            this);
    }

  public:
    ExpandableBlock() = default;
    ~ExpandableBlock() = default;

    ExpandableBlock(const ExpandableBlock &) = delete;
    ExpandableBlock &operator=(const ExpandableBlock &) = delete;

    ExpandableBlock &operator=(ExpandableBlock &&other) noexcept {
        if (this != &other) {
            LvglObject::operator=(std::move(other));
            header = std::move(other.header);
            container = std::move(other.container);
            content = std::move(other.content);
            expanded = other.expanded;
            other.expanded = false;
        }
        return *this;
    }

    void create(lv_obj_t *parent, lv_coord_t width, lv_coord_t height,
                lv_flex_flow_t flow) {
        lock();

        container.create(parent, width, height, LV_FLEX_FLOW_COLUMN);

        header.create(container.get(), this);
        header.setEventCallback([](lv_event_t *e, void *ctx) {
            if (lv_event_get_code(e) == LV_EVENT_CLICKED)
                static_cast<ExpandableBlock *>(ctx)->toggle();
        });

        content.create(container.get(), LV_PCT(100), LV_PCT(80), flow);
        expanded = false;
        updateContentVisibility();

        unlock();
    }

    void toggle() {
        lock();
        expanded = !expanded;
        updateContentVisibility();
        unlock();
    }

    void expand() {
        lock();
        expanded = true;
        updateContentVisibility();
        unlock();
    }

    void collapse() {
        lock();
        expanded = false;
        updateContentVisibility();
        unlock();
    }

    lv_obj_t *get() { return container.get(); }
    FlexContainer &getContainer() { return container; }
    FlexContainer &getContent() { return content; }
    lv_obj_t *getHeader() { return header.get(); }
};