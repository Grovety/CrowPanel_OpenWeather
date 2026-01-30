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
#include "i2c_bus.h"

using namespace esp_panel::drivers;
using namespace esp_panel::board;

static const char* TAG = "main";

#define I2C_NUM I2C_NUM_0
#define I2C_MASTER_SDA_IO GPIO_NUM_15 /*!< gpio number for I2C master clock */
#define I2C_MASTER_SCL_IO GPIO_NUM_16 /*!< gpio number for I2C master data  */
#define I2C_MASTER_FREQ_HZ 400000     /*!< I2C master clock frequency */

#define BACKLIGHT_ADDR_V1_1 0x30
#define BACKLIGHT_ADDR_V1_0 0x18

extern "C" void app_main(void)
{
    i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master =
            {
                .clk_speed = I2C_MASTER_FREQ_HZ,
            },
        .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL,
    };

    i2c_bus_handle_t i2c_bus = i2c_bus_create(I2C_NUM, &i2c_config);
    vTaskDelay(pdMS_TO_TICKS(50));

    bool is_v1_1 = false;

    static constexpr uint8_t addr_buf_sz = 10;
    uint8_t                  addr_buf[addr_buf_sz];

    ESP_LOGW(TAG, "Scan for I2C devices");

    uint8_t found_addrs = i2c_bus_scan(i2c_bus, addr_buf, addr_buf_sz);
    found_addrs         = std::min(addr_buf_sz, found_addrs);
    for (size_t i = 0; i < found_addrs; i++)
    {
        if (addr_buf[i] == BACKLIGHT_ADDR_V1_1)
        {
            is_v1_1 = true;
            break;
        }
    }

    if (is_v1_1)
    {
        i2c_bus_device_handle_t dev_v1_1 = i2c_bus_device_create(i2c_bus, BACKLIGHT_ADDR_V1_1, 0);
        if (!dev_v1_1)
        {
            ESP_LOGE(TAG, "Unable to create backlight control device");
        } else
        {
            esp_err_t err = i2c_bus_write_byte(dev_v1_1, NULL_I2C_MEM_ADDR, 0x10);
            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "Unable to configure backlight: %s", esp_err_to_name(err));
            }

            i2c_bus_device_delete(&dev_v1_1);
        }
    } else
    {
        ESP_LOGW(TAG, "Hardware V1.1 not found (0x30 missing). Assuming V1.0 hardware.");
    }

    if (!is_v1_1)
    {
        ESP_LOGW(TAG, "Initializing V1.0 Hardware. Switching to TCA9534 (0x18)...");

        i2c_bus_device_handle_t dev_v1_0 = i2c_bus_device_create(i2c_bus, BACKLIGHT_ADDR_V1_0, 0);

        if (dev_v1_0)
        {
            uint8_t config_reg = 0xFF;
            uint8_t output_reg = 0x00;

            if (i2c_bus_read_byte(dev_v1_0, 0x03, &config_reg) == ESP_OK)
            {
                config_reg &= ~(1 << 1);
                config_reg &= ~(1 << 2);
                config_reg &= ~(1 << 7);
                i2c_bus_write_byte(dev_v1_0, 0x03, config_reg);
            }

            i2c_bus_read_byte(dev_v1_0, 0x01, &output_reg);

            output_reg |= (1 << 1);
            output_reg |= (1 << 7);

            gpio_set_direction(GPIO_NUM_1, GPIO_MODE_OUTPUT);
            gpio_set_level(GPIO_NUM_1, 0);

            output_reg &= ~(1 << 2);
            i2c_bus_write_byte(dev_v1_0, 0x01, output_reg);

            vTaskDelay(pdMS_TO_TICKS(20));

            output_reg |= (1 << 2);
            i2c_bus_write_byte(dev_v1_0, 0x01, output_reg);

            vTaskDelay(pdMS_TO_TICKS(100));

            gpio_set_direction(GPIO_NUM_1, GPIO_MODE_INPUT);

            i2c_bus_write_byte(dev_v1_0, 0x01, output_reg);

            ESP_LOGW(TAG, "V1.0 initialized via TCA9534 (0x18)");

            i2c_bus_device_delete(&dev_v1_0);
        }
    }

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
        if (UseCases::LoadWeather::instance().perform())
        {
            Brightness::instance().update(CurrentTime::instance().isTimeSet() ? true : false);
            UseCases::LoadWeather::instance().timeout(10);
        } else
            vTaskDelay(1000);

        ESP_LOGI(TAG, "MALLOC_CAP_8BIT = %d", heap_caps_get_free_size(MALLOC_CAP_8BIT));
        ESP_LOGI(TAG, "MALLOC_CAP_SPIRAM = %d", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
        ESP_LOGI(TAG, "MALLOC_CAP_INTERNAL = %d",
                 heap_caps_get_free_size(MALLOC_CAP_INTERNAL)); // Heap free memory
    }
}
