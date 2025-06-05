#include "I2SMicrophone.h"

#include <esp_log.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>

#define MIC_BCLK_PIN GPIO_NUM_19
#define MIC_WS_PIN GPIO_NUM_2
#define MIC_DATA_IN_PIN GPIO_NUM_20

I2SMicrophone::I2SMicrophone()
{
}

I2SMicrophone::~I2SMicrophone()
{
    cleanup();
}

bool I2SMicrophone::init()
{
    ESP_LOGI(Tag, "Initializing I2S microphone...");

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG((i2s_port_t)i2sPort, I2S_ROLE_MASTER);
    chan_cfg.dma_desc_num      = dmaDescNum;
    chan_cfg.dma_frame_num     = dmaFrameNum;

    esp_err_t err = i2s_new_channel(&chan_cfg, nullptr, &rxChannel);
    if (err != ESP_OK)
    {
        ESP_LOGE(Tag, "Failed to create I2S channel: %s", esp_err_to_name(err));
        return false;
    }

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(sampleRate),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(
            I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = GPIO_NUM_NC,      // без внешней синхры
            .bclk = MIC_BCLK_PIN,
            .ws   = MIC_WS_PIN,
            .dout = GPIO_NUM_NC,      // только RX
            .din  = MIC_DATA_IN_PIN,
        },
    };

    err = i2s_channel_init_std_mode(rxChannel, &std_cfg);
    if (err != ESP_OK)
    {
        ESP_LOGE(Tag, "Failed to configure I2S channel: %s", esp_err_to_name(err));
        cleanup();
        return false;
    }

    err = i2s_channel_enable(rxChannel);
    if (err != ESP_OK)
    {
        ESP_LOGE(Tag, "Failed to enable I2S channel: %s", esp_err_to_name(err));
        cleanup();
        return false;
    }

    ESP_LOGI(Tag, "I2S microphone initialized");
    return true;
}

size_t I2SMicrophone::read(void* buffer, size_t size)
{
    if (!rxChannel || !buffer || size == 0)
        return 0;

    size_t    bytesRead = 0;
    esp_err_t err       = i2s_channel_read(rxChannel, buffer, size, &bytesRead, portMAX_DELAY);
    if (err != ESP_OK)
    {
        ESP_LOGW(Tag, "I2S read failed: %s", esp_err_to_name(err));
        return 0;
    }

    return bytesRead;
}

void I2SMicrophone::cleanup()
{
    if (rxChannel)
    {
        i2s_channel_disable(rxChannel);
        i2s_del_channel(rxChannel);
        rxChannel = nullptr;
    }
}

void I2SMicrophone::deinit()
{
    if (rxChannel)
    {
        ESP_LOGI(Tag, "Cleaning up I2S channel");
        i2s_channel_disable(rxChannel);
        i2s_del_channel(rxChannel);
        rxChannel = nullptr;
    }
}
