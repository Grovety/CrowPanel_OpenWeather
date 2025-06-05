#include "adapters/lvgl/lvgl_port_v8.h"

#include "entities/weather_screen/WeatherScreen.h"
#include "entities/wifi_screen/WifiScreen.h"
#include "entities/Brightness.h"
#include "entities/CurrentTime.h"
#include "entities/Location.h"
#include "entities/WIFI.h"
#include "entities/Weather.h"
#include "entities/Units.h"
#include "usecases/AccessPointsUpdate.h"
#include "usecases/LoadWeather.h"
#include <sys/time.h>
#include <time.h>
#include "WavRecorder.h"
#include "HttpServer.h"

using namespace esp_panel::drivers;
using namespace esp_panel::board;

static const char* TAG = "main";

extern "C" void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    Board*      board     = new Board();
    static bool connected = false;
    assert(board);

    ESP_UTILS_CHECK_FALSE_EXIT(board->init(), "Board init failed");
    ESP_UTILS_CHECK_FALSE_EXIT(board->begin(), "Board begin failed");
    ESP_UTILS_CHECK_FALSE_EXIT(lvgl_port_init(board->getLCD(), board->getTouch()),
                               "LVGL init failed");
    lv_disp_t*  dispp = lv_disp_get_default();
    lv_theme_t* theme =
        lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE),
                              lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);

    // create dashboard screen
    WifiScreen::instance().create(NULL);
    WeatherScreen::instance().create();
    WeatherScreen::instance().load();

    WIFI::instance().init();
    Brightness::instance().init();
    Units::instance().init();
    WifiScreen::instance().setUnits(Units::instance().getTemperature(),
                                    Units::instance().getPressure());
    UseCases::AccessPointsUpdate::instance().init();
    while (1)
    {
        if (!WIFI::instance().isConnected())
        {
            connected = false;
            WeatherScreen::instance().setSSID(NULL);
            WifiScreen::instance().setSSID(NULL, 0);
            WIFI::instance().waitForConnection();
            continue;
        }
        if ((!connected) && (!CurrentTime::instance().isTimeSet()))
        {
            CurrentTime::instance().init();
            if (CurrentTime::instance().sync())
            {
                time_t    now = CurrentTime::instance().now();
                struct tm timeinfo;
                localtime_r(&now, &timeinfo);
                char strftime_buf[64];
                strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
                ESP_LOGI(TAG, "The current date/time is: %s", strftime_buf);
            }
        }
        connected = true;
        UseCases::LoadWeather::instance().perform();
        Brightness::instance().update(CurrentTime::instance().isTimeSet() ? true : false);
        UseCases::LoadWeather::instance().timeout(10);

        ESP_LOGI(TAG, "MALLOC_CAP_8BIT = %d", heap_caps_get_free_size(MALLOC_CAP_8BIT));
        ESP_LOGI(TAG, "MALLOC_CAP_SPIRAM = %d", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
        ESP_LOGI(TAG, "MALLOC_CAP_INTERNAL = %d",
                 heap_caps_get_free_size(MALLOC_CAP_INTERNAL)); // Heap free memory
    }
}
