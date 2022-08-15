/* fastOLED example
 */

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "aniBuf.h"

static const char *TAG = "fastOLED";

#define I2C_MASTER_FREQ_HZ 1000000 /*!< I2C master clock frequency */ /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 50

#define I2C_ADDRESS 0x78

#define HEIGHT 64
#define WIDTH 128

uint8_t buffer[1024];
i2c_cmd_handle_t fullHandle = NULL;

esp_err_t setupI2c(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 21,
        .scl_io_num = 22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ};

    i2c_param_config(0, &conf);

    return i2c_driver_install(0, conf.mode, 0, 0, 0);
}

esp_err_t beginScreen(void)
{
    fullHandle = i2c_cmd_link_create();
    if (fullHandle == NULL)
    {
        ESP_LOGI(TAG, "Not enough memory");
        while (true)
        {
            ESP_LOGI(TAG, ".");
        }
    }
    ESP_ERROR_CHECK(i2c_master_start(fullHandle));
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, I2C_ADDRESS, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0x0, true));  // control byte
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0xAE, true)); // screen to sleep mode
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0xD5, true)); // Set refresh rate and divider
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0xF0, true)); //
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0xA8, true)); // Set number of columns multiplex ration
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0x3F, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0xD3, true)); // Set display offset
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0x0, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0x40, true)); // Set display start line check 40
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0x8D, true)); // Charge pump settings
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0x14, true)); //charge pump enable

    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0x20, true)); // Set memory addressing mode
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0x00, true)); // horizontal
    // trying column and page address, so resets properly
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0x21, true)); // Set column address register
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0x0, true));  // set start column to 0
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0x7F, true)); // set end column to 127 (since width is 128 pixels)

    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0x22, true)); // Set page address register
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0x0, true));  // set start column to 0
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0x7, true));  // set end column to 7 (since width is 8 pages or 8*8=64 pixels)

    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0xA1, true)); // Enable remapping of memory
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0xC8, true)); // Set comin direction
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0xDA, true)); // Set com pin configuration
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0x12, true)); // hardware compin config
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0x81, true)); // set contrast
    // ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0xB0, true)); // 176
    // ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0x01, true)); //1
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0xFF, true)); // 255
    // ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0x7F, true)); //127

    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0xD9, true)); // Set precharge
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0x22, true)); //Set both Phase 1 and phase 2 to 2 DCLK
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0xDB, true)); // Set vcom
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0x20, true)); //0.77v
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0xA4, true)); // display gddr
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0xA6, true)); // set inverse mode
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0x2E, true)); // Deactivate scroll
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0xAF, true)); // Turn on display
    ESP_ERROR_CHECK(i2c_master_stop(fullHandle));
    ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, fullHandle, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));
    i2c_cmd_link_delete(fullHandle);

    return ESP_OK;
}

void drawPixel(int x, int y, int colour)
{
    int byteLocation = ((y / 8) * WIDTH) + x;
    uint8_t newByte = buffer[byteLocation];
    if (colour)
    {
        newByte |= 1 << (y % 8);
    }
    else
    {
        newByte &= ~(1 << (y % 8));
    }

    buffer[byteLocation] = newByte;
}

void bufToScreen(void)
{
    fullHandle = i2c_cmd_link_create();
    if (fullHandle == NULL)
    {
        ESP_LOGI(TAG, "Not enough memory");
        while (true)
        {
            ESP_LOGI(TAG, ".");
        }
    }
    ESP_ERROR_CHECK(i2c_master_start(fullHandle));
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, I2C_ADDRESS, false));
    ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0x40, false)); // control byte
    ESP_ERROR_CHECK(i2c_master_write(fullHandle, buffer, 1024, false));
    ESP_ERROR_CHECK(i2c_master_stop(fullHandle));
    ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, fullHandle, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));
    i2c_cmd_link_delete(fullHandle);
}

void directToScreenAnimation(const unsigned char myAnimationBuf[][1024], int frames)
{
    for (int i = 0; i < frames; i++) // change with the number of frames
    {
        fullHandle = i2c_cmd_link_create();
        if (fullHandle == NULL)
        {
            ESP_LOGI(TAG, "Not enough memory");
            while (true)
            {
                ESP_LOGI(TAG, ".");
            }
        }
        ESP_ERROR_CHECK(i2c_master_start(fullHandle));
        ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, I2C_ADDRESS, false));
        ESP_ERROR_CHECK(i2c_master_write_byte(fullHandle, 0x40, false)); // control byte
        ESP_ERROR_CHECK(i2c_master_write(fullHandle, myAnimationBuf[i], 1024, false));
        ESP_ERROR_CHECK(i2c_master_stop(fullHandle));
        ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, fullHandle, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));
        i2c_cmd_link_delete(fullHandle);
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK(setupI2c()); // just the I2C config. No screen business
    ESP_LOGI(TAG, "Initialized successfully");
    ESP_ERROR_CHECK(beginScreen()); // basic screen config. No need to touch
    while (true)
    {
        for (int i = 0; i < 1024; i++)
        {
            buffer[i] = 0;
        }

        for (int i = 0; i < 64; i++)
        {
            drawPixel(0, i, 1);
            bufToScreen();
        }

        for (int i = 0; i < 128; i++)
        {
            drawPixel(i, 63, 1);
            bufToScreen();
        }

        for (int i = 63; i >= 0; i--)
        {
            drawPixel(127, i, 1);
            bufToScreen();
        }

        for (int i = 127; i >= 0; i--)
        {
            drawPixel(i, 0, 1);
            bufToScreen();
        }

        for (int i = 0; i < 5; i++)
        {
            directToScreenAnimation(fingerWalkAnimation, 37);
        }
        for (int i = 0; i < 5; i++)
        {
            directToScreenAnimation(fingerPrintAnimation, 37);
        }
        for (int i = 0; i < 5; i++)
        {
            directToScreenAnimation(churchAnimation, 161);
        }
        for (int i = 0; i < 5; i++)
        {
            directToScreenAnimation(pendriveAnimation, 103);
        }
    }
}
