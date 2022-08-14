/* i2c-tools example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "aniBuf.h"

static const char *TAG = "OLEDTest";

#define I2C_MASTER_FREQ_HZ 1000000 /*!< I2C master clock frequency */ /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 50

#define I2C_ADDRESS 0x78

#define HEIGHT 64
#define WIDTH 128
#define PAGE_SIZE 8
#define NUM_PAGES 8
#define NUM_ROWS_IN_PAGE 8

int togInverse = 0;


static uint8_t buffer[1024];
i2c_cmd_handle_t emptyHandle = NULL;
i2c_cmd_handle_t fullHandle = NULL;

static esp_err_t setupI2c(void)
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

static esp_err_t screenFlick(void)
{
    i2c_cmd_handle_t handle = i2c_cmd_link_create();
    if (handle == NULL)
    {
        ESP_LOGI(TAG, "Not enough memory");
        while (true)
        {
            ESP_LOGI(TAG, ".");
        }
    }
    ESP_ERROR_CHECK(i2c_master_start(handle));
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, I2C_ADDRESS, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xAE, true));

    ESP_ERROR_CHECK(i2c_master_stop(handle));
    ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, handle, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));
    i2c_cmd_link_delete(handle);

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    handle = i2c_cmd_link_create();
    if (handle == NULL)
    {
        ESP_LOGI(TAG, "Not enough memory");
        while (true)
        {
            ESP_LOGI(TAG, ".");
        }
    }
    ESP_ERROR_CHECK(i2c_master_start(handle));
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, I2C_ADDRESS, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xAF, true));

    ESP_ERROR_CHECK(i2c_master_stop(handle));
    ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, handle, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));
    i2c_cmd_link_delete(handle);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    return ESP_OK;
}

static esp_err_t beginScreen1(void)
{
    i2c_cmd_handle_t handle = i2c_cmd_link_create();
    if (handle == NULL)
    {
        ESP_LOGI(TAG, "Not enough memory");
        while (true)
        {
            ESP_LOGI(TAG, ".");
        }
    }
    ESP_ERROR_CHECK(i2c_master_start(handle));
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, I2C_ADDRESS, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x0, true));  // control byte
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xAE, true)); // screen to sleep mode
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xD5, true)); // Set refresh rate and divider
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xF0, true)); //
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xA8, true)); // Set number of columns multiplex ration
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x3F, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xD3, true)); // Set display offset
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x0, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x40, true)); // Set display start line check 40
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x8D, true)); // Charge pump settings
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x14, true));

    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x20, true)); // Set memory addressing mode
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x00, true)); // horizontal
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xA1, true)); // Enable remapping of memory
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xC8, true)); // Set comin direction
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xDA, true)); // Set com pin configuration
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x12, true)); //
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x81, true)); // set contrast
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xB0, true));

    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xD9, true)); // Set precharge
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x22, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xDB, true)); // Set vcom
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x20, true)); //
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xA4, true)); // display gddr
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xA6, true)); // set inverse mode
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x2E, true)); // Deactivate scroll
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xAF, true)); // Turn on display
    ESP_ERROR_CHECK(i2c_master_stop(handle));
    ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, handle, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));
    i2c_cmd_link_delete(handle);

    return ESP_OK;
}

static esp_err_t beginScreen2(void)
{
    i2c_cmd_handle_t handle = i2c_cmd_link_create();
    if (handle == NULL)
    {
        ESP_LOGI(TAG, "Not enough memory");
        while (true)
        {
            ESP_LOGI(TAG, ".");
        }
    }
    ESP_ERROR_CHECK(i2c_master_start(handle));
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, I2C_ADDRESS, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x0, true));  // control byte
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xAE, true)); // screen to sleep mode
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xD5, true)); // Set refresh rate and divider
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x90, true)); //
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xA8, true)); // Set number of columns multiplex ration
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x3F, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xD3, true)); // Set display offset
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x00, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x40, true)); // Set display start line check 40
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xA1, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xC8, true)); // Set comin direction
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xDA, true)); // Set com pin configuration
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x12, true)); //
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x81, true)); // set contrast
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xB0, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xD9, true)); // Set precharge
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x22, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xDB, true)); // Set vcom
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x30, true)); //
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xA4, true)); // display gddr
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xA6, true)); // set inverse mode

    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x8D, true)); // Charge pump settings
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x14, true)); // changing this to 14
    ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0xAF, true)); // Turn on display

    // ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x20, true)); //Set memory addressing mode
    // ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x00, true)); //horizontal

    // ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x2E, true)); //Deactivate scroll

    ESP_ERROR_CHECK(i2c_master_stop(handle));
    ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, handle, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));
    i2c_cmd_link_delete(handle);

    return ESP_OK;
}

void writeToScreen(void)
{

    // i2c_cmd_handle_t handle = i2c_cmd_link_create();
    // if (handle == NULL){
    //     ESP_LOGI(TAG, "Not enough memory");
    //     while(true){
    //         ESP_LOGI(TAG, ".");
    //     }
    // }
    // ESP_ERROR_CHECK(i2c_master_start(handle));
    // ESP_ERROR_CHECK(i2c_master_write_byte(handle, I2C_ADDRESS, false));
    // ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x0, false)); //control byte
    // ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x22, false)); // Set page address start and end
    // ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x0, false));
    // ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x3F, false));
    // ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x21, false)); // Set column address start and end
    // ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x0, false));
    // ESP_ERROR_CHECK(i2c_master_write_byte(handle, 0x7F, false));
    // ESP_ERROR_CHECK(i2c_master_stop(handle));
    // ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, handle, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));
    // i2c_cmd_link_delete(handle);

    // if(togInverse){
    //     ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, fullHandle, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));
    // }else{
    //     ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, emptyHandle, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));
    // }
    ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, fullHandle, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // i2c_cmd_link_delete(handle);
}

void writeBuffer(uint8_t *buffer)
{
    // do all setup for each screen write

    for (int pageGroup = 0; pageGroup < NUM_PAGES; pageGroup++)
    { // for each page row (horizontal rectangular collection) //indicates row number multiplier

        for (int rowGroup = 0; rowGroup < WIDTH / 8; rowGroup++)
        { // for each byte column in a row (vertical rectangle in page) //indicates col number multiplier

            for (int bitPosition = 7; bitPosition >= 0; bitPosition--)
            { // for each bit position in a byte. Going MSB first (thinner rectangle) //indicates bit col number

                uint8_t convertedByte = 0x0;

                for (int byteNum = 0; byteNum < NUM_ROWS_IN_PAGE; byteNum++)
                { // each speciifc bit //indicates bit row number

                    convertedByte |= ((buffer[(((pageGroup * 8) + byteNum) * (WIDTH / 8)) + rowGroup] >> bitPosition) & 0x01) << byteNum;
                    // printf("%X\n", ((buffer[(((pageGroup*8) + byteNum)*(WIDTH/8)) + rowGroup] >> bitPosition) & 0x01) << byteNum);
                    // printf("%X\n", convertedByte);
                    // vTaskDelay(10/portTICK_PERIOD_MS);
                }
                // converted byte is complete here

                ESP_LOGI(TAG, "%X", convertedByte);
                vTaskDelay(10000 / portTICK_PERIOD_MS);
            }
        }
    }
}

void writeToBuffer(int x, int y, int colour)
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

void bufToScreenAnimation(void)
{
    for (int i = 0; i < 41; i++)
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
        ESP_ERROR_CHECK(i2c_master_write(fullHandle, bufferAnimation[i], 1024, false));
        ESP_ERROR_CHECK(i2c_master_stop(fullHandle));
        ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, fullHandle, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));
        i2c_cmd_link_delete(fullHandle);
    }
}

void app_main(void)
{

    // for (int i = 0; i < 1024; i++)
    // {
    //     buffer[i] = 0x0;
    // }

    ESP_ERROR_CHECK(setupI2c()); // just the I2C config. No screen business
    ESP_LOGI(TAG, "Initialized successfully");
    ESP_ERROR_CHECK(beginScreen1()); // basic screen config. No need to touch
    while (true)
    {
        // ESP_LOGI(TAG, "Start");
        // for (int i = 0; i < 128; i++)
        // {
        //     for (int j = 0; j < 64; j++)
        //     {
        //         writeToBuffer(i, j, togInverse);
        //     }
        // }

        // bufToScreen();
        // togInverse = (togInverse == 1 ? 0 : 1);

        bufToScreenAnimation();
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}
