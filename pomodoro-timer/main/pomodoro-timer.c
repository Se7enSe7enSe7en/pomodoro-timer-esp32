/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/spi_master.h"

static const char *TAG = "POMODORO";

// =============================================================================
// Pin Definitions (from board.h)
// =============================================================================
#define LCD_SPI_DATA0     48  // MOSI
#define LCD_SPI_CLK       45
#define LCD_SPI_CS        38
#define LCD_PIN_RST       41
#define LCD_PIN_BK_LIGHT  5

#define LCD_VSYNC_GPIO    41  // Wait, RST is also 41? Let me check board.h again.
// board.h says: #define LCD_PIN_RST 41 AND #define LCD_VSYNC_GPIO 41
// This implies they share the pin? Or maybe VSYNC is used as RST in SPI mode?
// In screen.c:
// gpio_config_t rst_gpio_config = { .pin_bit_mask = 1ULL << LCD_PIN_RST ... };
// Then it toggles RST.
// AND THEN: .vsync_gpio_num = LCD_VSYNC_GPIO in panel_config.
// See manual note: "LCD_VSYNC_GPIO (41)" and "LCD_PIN_RST 41".
// It seems pin 41 is multiplexed? Or maybe there's a mistake in my reading?
// Let's re-read board.h carefully.
// board.h: #define LCD_PIN_RST       41
// board.h: #define LCD_VSYNC_GPIO    (41)
// This is weird. RGB interface needs VSYNC.
// Maybe after SPI init (where RST might be used or ignored), it becomes VSYNC?
// screen.c toggles RST BEFORE creating RGB panel.
// So yes, it toggles pin 41 as GPIO, then hands it over to RGB panel as VSYNC.

#define LCD_HSYNC_GPIO    42
#define LCD_DE_GPIO       40
#define LCD_PCLK_GPIO     39
#define LCD_DISP_EN_GPIO  -1

#define LCD_DATA0_GPIO    45 // B0 - Wait, 45 is also CLK?
#define LCD_DATA1_GPIO    48 // B1 - Wait, 48 is also DATA0 (MOSI)?
#define LCD_DATA2_GPIO    47 // B2
#define LCD_DATA3_GPIO    0  // B3
#define LCD_DATA4_GPIO    21 // B4
#define LCD_DATA5_GPIO    14 // G0
#define LCD_DATA6_GPIO    13 // G1
#define LCD_DATA7_GPIO    12 // G2
#define LCD_DATA8_GPIO    11 // G3
#define LCD_DATA9_GPIO    16 // G4
#define LCD_DATA10_GPIO   17 // G5
#define LCD_DATA11_GPIO   18 // R0
#define LCD_DATA12_GPIO   8  // R1
#define LCD_DATA13_GPIO   3  // R2
#define LCD_DATA14_GPIO   46 // R3
#define LCD_DATA15_GPIO   10 // R4

#define LCD_BK_LIGHT_ON_LEVEL  1
#define LCD_BK_LIGHT_OFF_LEVEL 0

// =============================================================================
// SPI Initialization Logic (from screen.c)
// =============================================================================
static spi_device_handle_t g_screen_spi;

static void __spi_send_cmd(uint8_t cmd)
{
    uint16_t tmp_cmd = (cmd | 0x0000);
    spi_transaction_ext_t trans = (spi_transaction_ext_t){
        .base = {
            .flags = SPI_TRANS_VARIABLE_CMD,
            .cmd = tmp_cmd,
        },
        .command_bits = 9,
    };
    spi_device_transmit(g_screen_spi, (spi_transaction_t *)&trans);
}

static void __spi_send_data(uint8_t data)
{
    uint16_t tmp_data = (data | 0x0100);
    spi_transaction_ext_t trans = (spi_transaction_ext_t){
        .base = {
            .flags = SPI_TRANS_VARIABLE_CMD,
            .cmd = tmp_data,
        },
        .command_bits = 9,
    };
    spi_device_transmit(g_screen_spi, (spi_transaction_t *)&trans);
}

static void rgb_driver_init(void)
{
    // [Init sequence copied from screen.c]
    __spi_send_cmd (0xF0); __spi_send_data (0x55); __spi_send_data (0xAA); __spi_send_data (0x52); __spi_send_data (0x08); __spi_send_data (0x00);
    __spi_send_cmd (0xF6); __spi_send_data (0x5A); __spi_send_data (0x87);
    __spi_send_cmd (0xC1); __spi_send_data (0x3F);
    __spi_send_cmd (0xC2); __spi_send_data (0x0E);
    __spi_send_cmd (0xC6); __spi_send_data (0xF8);
    __spi_send_cmd (0xC9); __spi_send_data (0x10);
    __spi_send_cmd (0xCD); __spi_send_data (0x25);
    __spi_send_cmd (0xF8); __spi_send_data (0x8A);
    __spi_send_cmd (0xAC); __spi_send_data (0x45);
    __spi_send_cmd (0xA0); __spi_send_data (0xDD);
    __spi_send_cmd (0xA7); __spi_send_data (0x47);
    __spi_send_cmd (0xFA); __spi_send_data (0x00); __spi_send_data (0x00); __spi_send_data (0x00); __spi_send_data (0x04);
    __spi_send_cmd (0x86); __spi_send_data (0x99); __spi_send_data (0xa3); __spi_send_data (0xa3); __spi_send_data (0x51);
    __spi_send_cmd (0xA3); __spi_send_data (0xEE);
    __spi_send_cmd (0xFD); __spi_send_data (0x3c); __spi_send_data (0x3c); __spi_send_data (0x00);
    __spi_send_cmd (0x71); __spi_send_data (0x48);
    __spi_send_cmd (0x72); __spi_send_data (0x48);
    __spi_send_cmd (0x73); __spi_send_data (0x00); __spi_send_data (0x44);
    __spi_send_cmd (0x97); __spi_send_data (0xEE);
    __spi_send_cmd (0x83); __spi_send_data (0x93);
    __spi_send_cmd (0x9A); __spi_send_data (0x72);
    __spi_send_cmd (0x9B); __spi_send_data (0x5a);
    __spi_send_cmd (0x82); __spi_send_data (0x2c); __spi_send_data (0x2c);
    __spi_send_cmd (0xB1); __spi_send_data (0x10);
    __spi_send_cmd (0x6D); __spi_send_data (0x00); __spi_send_data (0x1F); __spi_send_data (0x19); __spi_send_data (0x1A); __spi_send_data (0x10); __spi_send_data (0x0e); __spi_send_data (0x0c); __spi_send_data (0x0a); __spi_send_data (0x02); __spi_send_data (0x07); __spi_send_data (0x1E); __spi_send_data (0x1E); __spi_send_data (0x1E); __spi_send_data (0x1E); __spi_send_data (0x1E); __spi_send_data (0x1E); __spi_send_data (0x1E); __spi_send_data (0x1E); __spi_send_data (0x1E); __spi_send_data (0x1E); __spi_send_data (0x1E); __spi_send_data (0x1E); __spi_send_data (0x08); __spi_send_data (0x01); __spi_send_data (0x09); __spi_send_data (0x0b); __spi_send_data (0x0D); __spi_send_data (0x0F); __spi_send_data (0x1a); __spi_send_data (0x19); __spi_send_data (0x1f); __spi_send_data (0x00);
    __spi_send_cmd (0x64); __spi_send_data (0x38); __spi_send_data (0x05); __spi_send_data (0x01); __spi_send_data (0xdb); __spi_send_data (0x03); __spi_send_data (0x03); __spi_send_data (0x38); __spi_send_data (0x04); __spi_send_data (0x01); __spi_send_data (0xdc); __spi_send_data (0x03); __spi_send_data (0x03); __spi_send_data (0x7A); __spi_send_data (0x7A); __spi_send_data (0x7A); __spi_send_data (0x7A);
    __spi_send_cmd (0x65); __spi_send_data (0x38); __spi_send_data (0x03); __spi_send_data (0x01); __spi_send_data (0xdd); __spi_send_data (0x03); __spi_send_data (0x03); __spi_send_data (0x38); __spi_send_data (0x02); __spi_send_data (0x01); __spi_send_data (0xde); __spi_send_data (0x03); __spi_send_data (0x03); __spi_send_data (0x7A); __spi_send_data (0x7A); __spi_send_data (0x7A); __spi_send_data (0x7A);
    __spi_send_cmd (0x66); __spi_send_data (0x38); __spi_send_data (0x01); __spi_send_data (0x01); __spi_send_data (0xdf); __spi_send_data (0x03); __spi_send_data (0x03); __spi_send_data (0x38); __spi_send_data (0x00); __spi_send_data (0x01); __spi_send_data (0xe0); __spi_send_data (0x03); __spi_send_data (0x03); __spi_send_data (0x7A); __spi_send_data (0x7A); __spi_send_data (0x7A); __spi_send_data (0x7A);
    __spi_send_cmd (0x67); __spi_send_data (0x30); __spi_send_data (0x01); __spi_send_data (0x01); __spi_send_data (0xe1); __spi_send_data (0x03); __spi_send_data (0x03); __spi_send_data (0x30); __spi_send_data (0x02); __spi_send_data (0x01); __spi_send_data (0xe2); __spi_send_data (0x03); __spi_send_data (0x03); __spi_send_data (0x7A); __spi_send_data (0x7A); __spi_send_data (0x7A); __spi_send_data (0x7A);
    __spi_send_cmd (0x68); __spi_send_data (0x00); __spi_send_data (0x08); __spi_send_data (0x15); __spi_send_data (0x08); __spi_send_data (0x15); __spi_send_data (0x7A); __spi_send_data (0x7A); __spi_send_data (0x08); __spi_send_data (0x15); __spi_send_data (0x08); __spi_send_data (0x15); __spi_send_data (0x7A); __spi_send_data (0x7A);
    __spi_send_cmd (0x60); __spi_send_data (0x38); __spi_send_data (0x08); __spi_send_data (0x7A); __spi_send_data (0x7A); __spi_send_data (0x38); __spi_send_data (0x09); __spi_send_data (0x7A); __spi_send_data (0x7A);
    __spi_send_cmd (0x63); __spi_send_data (0x31); __spi_send_data (0xe4); __spi_send_data (0x7A); __spi_send_data (0x7A); __spi_send_data (0x31); __spi_send_data (0xe5); __spi_send_data (0x7A); __spi_send_data (0x7A);
    __spi_send_cmd (0x69); __spi_send_data (0x04); __spi_send_data (0x22); __spi_send_data (0x14); __spi_send_data (0x22); __spi_send_data (0x14); __spi_send_data (0x22); __spi_send_data (0x08);
    __spi_send_cmd (0x6B); __spi_send_data (0x07);
    __spi_send_cmd (0x7A); __spi_send_data (0x08); __spi_send_data (0x13);
    __spi_send_cmd (0x7B); __spi_send_data (0x08); __spi_send_data (0x13);
    
    // The sequence continues... this is a lot of hex.
    // I will include the rest of the chunks from the observation.
    // Chunk 2 end + Chunk 3 start:
    __spi_send_cmd (0xD1); __spi_send_data (0x00); __spi_send_data (0x00); __spi_send_data (0x00); __spi_send_data (0x04); __spi_send_data (0x00); __spi_send_data (0x12); __spi_send_data (0x00); __spi_send_data (0x18); __spi_send_data (0x00); __spi_send_data (0x21); __spi_send_data (0x00); __spi_send_data (0x2a); __spi_send_data (0x00); __spi_send_data (0x35); __spi_send_data (0x00); __spi_send_data (0x47); __spi_send_data (0x00); __spi_send_data (0x56); __spi_send_data (0x00); __spi_send_data (0x90); __spi_send_data (0x00); __spi_send_data (0xe5); __spi_send_data (0x01); __spi_send_data (0x68); __spi_send_data (0x01); __spi_send_data (0xd5); __spi_send_data (0x01); __spi_send_data (0xd7); __spi_send_data (0x02); __spi_send_data (0x36); __spi_send_data (0x02);
    __spi_send_data (0xa6); __spi_send_data (0x02); __spi_send_data (0xee); __spi_send_data (0x03); __spi_send_data (0x48); __spi_send_data (0x03); __spi_send_data (0xa0); __spi_send_data (0x03); __spi_send_data (0xba); __spi_send_data (0x03); __spi_send_data (0xc5); __spi_send_data (0x03); __spi_send_data (0xd0); __spi_send_data (0x03); __spi_send_data (0xE0); __spi_send_data (0x03); __spi_send_data (0xea); __spi_send_data (0x03); __spi_send_data (0xFa); __spi_send_data (0x03); __spi_send_data (0xFF);

    __spi_send_cmd (0xD2); __spi_send_data (0x00); __spi_send_data (0x00); __spi_send_data (0x00); __spi_send_data (0x04); __spi_send_data (0x00); __spi_send_data (0x12); __spi_send_data (0x00); __spi_send_data (0x18); __spi_send_data (0x00); __spi_send_data (0x21); __spi_send_data (0x00); __spi_send_data (0x2a); __spi_send_data (0x00); __spi_send_data (0x35); __spi_send_data (0x00); __spi_send_data (0x47); __spi_send_data (0x00); __spi_send_data (0x56); __spi_send_data (0x00); __spi_send_data (0x90); __spi_send_data (0x00); __spi_send_data (0xe5); __spi_send_data (0x01); __spi_send_data (0x68); __spi_send_data (0x01); __spi_send_data (0xd5); __spi_send_data (0x01); __spi_send_data (0xd7); __spi_send_data (0x02); __spi_send_data (0x36); __spi_send_data (0x02); __spi_send_data (0xa6); __spi_send_data (0x02); __spi_send_data (0xee); __spi_send_data (0x03); __spi_send_data (0x48); __spi_send_data (0x03); __spi_send_data (0xa0); __spi_send_data (0x03); __spi_send_data (0xba); __spi_send_data (0x03); __spi_send_data (0xc5); __spi_send_data (0x03); __spi_send_data (0xd0); __spi_send_data (0x03); __spi_send_data (0xE0); __spi_send_data (0x03); __spi_send_data (0xea); __spi_send_data (0x03); __spi_send_data (0xFa); __spi_send_data (0x03); __spi_send_data (0xFF);

    __spi_send_cmd (0xD3); __spi_send_data (0x00); __spi_send_data (0x00); __spi_send_data (0x00); __spi_send_data (0x04); __spi_send_data (0x00); __spi_send_data (0x12); __spi_send_data (0x00); __spi_send_data (0x18); __spi_send_data (0x00); __spi_send_data (0x21); __spi_send_data (0x00); __spi_send_data (0x2a); __spi_send_data (0x00); __spi_send_data (0x35); __spi_send_data (0x00); __spi_send_data (0x47); __spi_send_data (0x00); __spi_send_data (0x56); __spi_send_data (0x00); __spi_send_data (0x90); __spi_send_data (0x00); __spi_send_data (0xe5); __spi_send_data (0x01); __spi_send_data (0x68); __spi_send_data (0x01); __spi_send_data (0xd5); __spi_send_data (0x01); __spi_send_data (0xd7); __spi_send_data (0x02); __spi_send_data (0x36); __spi_send_data (0x02); __spi_send_data (0xa6); __spi_send_data (0x02); __spi_send_data (0xee); __spi_send_data (0x03); __spi_send_data (0x48); __spi_send_data (0x03); __spi_send_data (0xa0); __spi_send_data (0x03); __spi_send_data (0xba); __spi_send_data (0x03); __spi_send_data (0xc5); __spi_send_data (0x03); __spi_send_data (0xd0); __spi_send_data (0x03); __spi_send_data (0xE0); __spi_send_data (0x03); __spi_send_data (0xea); __spi_send_data (0x03); __spi_send_data (0xFa); __spi_send_data (0x03); __spi_send_data (0xFF);

    __spi_send_cmd (0xD4); __spi_send_data (0x00); __spi_send_data (0x00); __spi_send_data (0x00); __spi_send_data (0x04); __spi_send_data (0x00); __spi_send_data (0x12); __spi_send_data (0x00); __spi_send_data (0x18); __spi_send_data (0x00); __spi_send_data (0x21); __spi_send_data (0x00); __spi_send_data (0x2a); __spi_send_data (0x00); __spi_send_data (0x35); __spi_send_data (0x00); __spi_send_data (0x47); __spi_send_data (0x00); __spi_send_data (0x56); __spi_send_data (0x00); __spi_send_data (0x90); __spi_send_data (0x00); __spi_send_data (0xe5); __spi_send_data (0x01); __spi_send_data (0x68); __spi_send_data (0x01); __spi_send_data (0xd5); __spi_send_data (0x01); __spi_send_data (0xd7); __spi_send_data (0x02); __spi_send_data (0x36); __spi_send_data (0x02); __spi_send_data (0xa6); __spi_send_data (0x02); __spi_send_data (0xee); __spi_send_data (0x03); __spi_send_data (0x48); __spi_send_data (0x03); __spi_send_data (0xa0); __spi_send_data (0x03); __spi_send_data (0xba); __spi_send_data (0x03); __spi_send_data (0xc5); __spi_send_data (0x03); __spi_send_data (0xd0); __spi_send_data (0x03); __spi_send_data (0xE0); __spi_send_data (0x03); __spi_send_data (0xea); __spi_send_data (0x03); __spi_send_data (0xFa); __spi_send_data (0x03); __spi_send_data (0xFF);

    __spi_send_cmd (0xD5); __spi_send_data (0x00); __spi_send_data (0x00); __spi_send_data (0x00); __spi_send_data (0x04); __spi_send_data (0x00); __spi_send_data (0x12); __spi_send_data (0x00); __spi_send_data (0x18); __spi_send_data (0x00); __spi_send_data (0x21); __spi_send_data (0x00); __spi_send_data (0x2a); __spi_send_data (0x00); __spi_send_data (0x35); __spi_send_data (0x00); __spi_send_data (0x47); __spi_send_data (0x00); __spi_send_data (0x56); __spi_send_data (0x00); __spi_send_data (0x90); __spi_send_data (0x00); __spi_send_data (0xe5); __spi_send_data (0x01); __spi_send_data (0x68); __spi_send_data (0x01); __spi_send_data (0xd5); __spi_send_data (0x01); __spi_send_data (0xd7); __spi_send_data (0x02); __spi_send_data (0x36); __spi_send_data (0x02); __spi_send_data (0xa6); __spi_send_data (0x02); __spi_send_data (0xee); __spi_send_data (0x03); __spi_send_data (0x48); __spi_send_data (0x03); __spi_send_data (0xa0); __spi_send_data (0x03); __spi_send_data (0xba); __spi_send_data (0x03); __spi_send_data (0xc5); __spi_send_data (0x03); __spi_send_data (0xd0); __spi_send_data (0x03); __spi_send_data (0xE0); __spi_send_data (0x03); __spi_send_data (0xea); __spi_send_data (0x03); __spi_send_data (0xFa); __spi_send_data (0x03); __spi_send_data (0xFF);

    __spi_send_cmd (0xD6); __spi_send_data (0x00); __spi_send_data (0x00); __spi_send_data (0x00); __spi_send_data (0x04); __spi_send_data (0x00); __spi_send_data (0x12); __spi_send_data (0x00); __spi_send_data (0x18); __spi_send_data (0x00); __spi_send_data (0x21); __spi_send_data (0x00); __spi_send_data (0x2a); __spi_send_data (0x00); __spi_send_data (0x35); __spi_send_data (0x00); __spi_send_data (0x47); __spi_send_data (0x00); __spi_send_data (0x56); __spi_send_data (0x00); __spi_send_data (0x90); __spi_send_data (0x00); __spi_send_data (0xe5); __spi_send_data (0x01); __spi_send_data (0x68); __spi_send_data (0x01); __spi_send_data (0xd5); __spi_send_data (0x01); __spi_send_data (0xd7); __spi_send_data (0x02); __spi_send_data (0x36); __spi_send_data (0x02); __spi_send_data (0xa6); __spi_send_data (0x02); __spi_send_data (0xee); __spi_send_data (0x03); __spi_send_data (0x48); __spi_send_data (0x03); __spi_send_data (0xa0); __spi_send_data (0x03); __spi_send_data (0xba); __spi_send_data (0x03); __spi_send_data (0xc5); __spi_send_data (0x03); __spi_send_data (0xd0); __spi_send_data (0x03); __spi_send_data (0xE0); __spi_send_data (0x03); __spi_send_data (0xea); __spi_send_data (0x03); __spi_send_data (0xFa); __spi_send_data (0x03); __spi_send_data (0xFF);

    __spi_send_cmd (0x3a); __spi_send_data (0x66);
    __spi_send_cmd (0x11);
    vTaskDelay(pdMS_TO_TICKS(120));
    __spi_send_cmd (0x29);
    vTaskDelay(pdMS_TO_TICKS(20));
}

void qmsd_rgb_spi_init() {
    spi_bus_config_t buscfg = {
        .sclk_io_num = LCD_SPI_CLK,
        .mosi_io_num = LCD_SPI_DATA0,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 10 * 1024,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = SPI_MASTER_FREQ_10M,   // Clock out at 10 MHz
        .mode = 0,                               // SPI mode 0
        .spics_io_num = LCD_SPI_CS,              // CS pin
        .queue_size = 7,                         // We want to be able to queue 7 transactions at a time
    };

    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &g_screen_spi));
    rgb_driver_init(); // Send magic sequence

    spi_bus_remove_device(g_screen_spi);
    spi_bus_free(SPI2_HOST);
}

// =============================================================================
// Main Display Init Function
// =============================================================================
esp_lcd_panel_handle_t panel_handle = NULL;

void app_main(void)
{
    ESP_LOGI(TAG, "Initializing Display...");

    // 1. Reset and Backlight Init
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << LCD_PIN_BK_LIGHT
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
    ESP_ERROR_CHECK(gpio_set_level(LCD_PIN_BK_LIGHT, LCD_BK_LIGHT_OFF_LEVEL));

    gpio_config_t rst_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << LCD_PIN_RST
    };
    ESP_ERROR_CHECK(gpio_config(&rst_gpio_config));

    // Reset sequence
    ESP_ERROR_CHECK(gpio_set_level(LCD_PIN_RST, 1));
    vTaskDelay(pdMS_TO_TICKS(10));
    ESP_ERROR_CHECK(gpio_set_level(LCD_PIN_RST, 0));
    vTaskDelay(pdMS_TO_TICKS(50));
    ESP_ERROR_CHECK(gpio_set_level(LCD_PIN_RST, 1));
    vTaskDelay(pdMS_TO_TICKS(100));

    // 2. SPI Config Init (Manual 9-bit SPI)
    qmsd_rgb_spi_init();

    // 3. RGB Panel Init
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16,
        .psram_trans_align = 64,
        .pclk_gpio_num = LCD_PCLK_GPIO,
        .vsync_gpio_num = LCD_VSYNC_GPIO,
        .hsync_gpio_num = LCD_HSYNC_GPIO,
        .de_gpio_num = LCD_DE_GPIO,
        .disp_gpio_num = LCD_DISP_EN_GPIO,
        .data_gpio_nums = {
            LCD_DATA0_GPIO, LCD_DATA1_GPIO, LCD_DATA2_GPIO, LCD_DATA3_GPIO,
            LCD_DATA4_GPIO, LCD_DATA5_GPIO, LCD_DATA6_GPIO, LCD_DATA7_GPIO,
            LCD_DATA8_GPIO, LCD_DATA9_GPIO, LCD_DATA10_GPIO, LCD_DATA11_GPIO,
            LCD_DATA12_GPIO, LCD_DATA13_GPIO, LCD_DATA14_GPIO, LCD_DATA15_GPIO,
        },
        .timings = {
            .pclk_hz = 15000000,
            .h_res = 480,
            .v_res = 480,
            .hsync_pulse_width = 10,
            .hsync_back_porch = 40,
            .hsync_front_porch = 8,
            .vsync_pulse_width = 10,
            .vsync_back_porch = 40,
            .vsync_front_porch = 8,
        },
        .flags.fb_in_psram = 1,
        .clk_src = LCD_CLK_SRC_PLL160M,
    };

    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    
    // Turn on backlight
    ESP_ERROR_CHECK(gpio_set_level(LCD_PIN_BK_LIGHT, LCD_BK_LIGHT_ON_LEVEL));

    // 4. Test Draw: Red Screen
    ESP_LOGI(TAG, "Drawing Red Screen Test...");
    uint16_t *buffer = malloc(480 * sizeof(uint16_t)); // One line buffer
    if (buffer) {
        for (int i = 0; i < 480; i++) {
            buffer[i] = 0xF800; // Red in RGB565
        }
        for (int y = 0; y < 480; y++) {
            esp_lcd_panel_draw_bitmap(panel_handle, 0, y, 480, y+1, buffer);
        }
        free(buffer);
    } else {
        ESP_LOGE(TAG, "Failed to allocate draw buffer");
    }

    ESP_LOGI(TAG, "Display Test Complete.");
}
