/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#pragma once

#include "sdkconfig.h"
#include "esp_display_panel.hpp"
#include "lvgl.h"

// *INDENT-OFF*

/**
 * LVGL related parameters, can be adjusted by users
 */
#define LVGL_PORT_TICK_PERIOD_MS (2) // The period of the LVGL tick task, in milliseconds

/**
 *
 * LVGL buffer related parameters, can be adjusted by users:
 *
 *  (These parameters will be useless if the avoid tearing function is enabled)
 *
 *  - Memory type for buffer allocation:
 *      - MALLOC_CAP_SPIRAM: Allocate LVGL buffer in PSRAM
 *      - MALLOC_CAP_INTERNAL: Allocate LVGL buffer in SRAM
 *
 *      (The SRAM is faster than PSRAM, but the PSRAM has a larger capacity)
 *      (For SPI/QSPI LCD, it is recommended to allocate the buffer in SRAM,
 * because the SPI DMA does not directly support PSRAM now)
 *
 *  - The size (in bytes) and number of buffers:
 *      - Lager buffer size can improve FPS, but it will occupy more memory.
 * Maximum buffer size is `width * height`.
 *      - The number of buffers should be 1 or 2.
 */
// #define LVGL_PORT_BUFFER_MALLOC_CAPS  (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
// // Allocate LVGL buffer in SRAM
#define LVGL_PORT_BUFFER_MALLOC_CAPS (MALLOC_CAP_SPIRAM) //
// Allocate LVGL buffer in PSRAM
#define LVGL_PORT_BUFFER_SIZE_HEIGHT (20)
#define LVGL_PORT_BUFFER_NUM (2)

/**
 * LVGL timer handle task related parameters, can be adjusted by users
 */
#define LVGL_PORT_TASK_MAX_DELAY_MS                                                                \
    (500)                               // The maximum delay of the LVGL timer task, in milliseconds
#define LVGL_PORT_TASK_MIN_DELAY_MS (2) // The minimum delay of the LVGL timer task, in milliseconds
#define LVGL_PORT_TASK_STACK_SIZE (6 * 1024) // The stack size of the LVGL timer task, in bytes
#define LVGL_PORT_TASK_PRIORITY (2)          // The priority of the LVGL timer task
#define LVGL_PORT_TASK_CORE (0)
// The core of the LVGL timer task, `-1` means the don't specify the core
// Default is the same as the main core
// This can be set to `1` only if the SoCs support dual-core,
// otherwise it should be set to `-1` or `0`

// *INDENT-ON*

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Porting LVGL with LCD and touch panel. This function should be called
 * after the initialization of the LCD and touch panel.
 *
 * @param lcd The pointer to the LCD panel device, mustn't be nullptr
 * @param tp  The pointer to the touch panel device, set to nullptr if is not
 * used
 *
 * @return true if success, otherwise false
 */
bool lvgl_port_init(esp_panel::drivers::LCD* lcd, esp_panel::drivers::Touch* tp);

/**
 * @brief Deinitialize the LVGL porting.
 *
 * @return true if success, otherwise false
 */
bool lvgl_port_deinit(void);

/**
 * @brief Lock the LVGL mutex. This function should be called before calling any
 * LVGL APIs when not in LVGL task, and the `lvgl_port_unlock()` function should
 * be called later.
 *
 * @param timeout_ms The timeout of the mutex lock, in milliseconds. If the
 * timeout is set to `-1`, it will wait indefinitely.
 *
 * @return true if success, otherwise false
 */
bool lvgl_port_lock(int timeout_ms = -1);

/**
 * @brief Unlock the LVGL mutex. This function should be called after using LVGL
 * APIs when not in LVGL task, and the `lvgl_port_lock()` function should be
 * called before.
 *
 * @return true if success, otherwise false
 */
bool lvgl_port_unlock(void);

#ifdef __cplusplus
}
#endif
