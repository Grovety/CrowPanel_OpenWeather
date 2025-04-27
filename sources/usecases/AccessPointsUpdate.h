#pragma once

#include "freertos/FreeRTOS.h"
#ifdef COMMON_DEMO_APP
#include "entities/ui/wifi_screen/WifiScreen.h"
#include "entities/ui/wifi_screen/elements/AccessPointItem.h"
#else
#include "entities/wifi_screen/WifiScreen.h"
#include "entities/wifi_screen/elements/AccessPointItem.h"
#endif

namespace UseCases
{
class AccessPointsUpdate
{
    static constexpr int      TaskSize        = 4096;
    static constexpr int      TaskPriority    = 0;
    static constexpr char*    TaskName        = "APUpd";
    static constexpr char*    Tag             = "AccessPointUpdate";
    static constexpr uint32_t UpdatePersiodMs = 60000;

    TaskHandle_t                  taskHandle = nullptr;
    static void                   task(void*);
    std::vector<AccessPointItem*> wifiList;

public:
    static AccessPointsUpdate& instance()
    {
        static AccessPointsUpdate instance;
        return instance;
    }
    bool init();
};
} // namespace UseCases