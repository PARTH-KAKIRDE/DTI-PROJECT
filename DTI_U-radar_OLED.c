#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "ssd1306.h"

#define TRIG_PIN GPIO_NUM_4
#define ECHO_PIN GPIO_NUM_2

#define I2C_SDA_PIN GPIO_NUM_21
#define I2C_SCL_PIN GPIO_NUM_22

#define SSD1306_ADDR 0x3C
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

static const char *TAG = "ultrasonic_radar";

void ultrasonic_radar_task(void *pvParameters) {
    gpio_pad_select_gpio(TRIG_PIN);
    gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(ECHO_PIN);
    gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);

    i2c_master_init(I2C_SDA_PIN, I2C_SCL_PIN);

    ssd1306_configure(SSD1306_ADDR, SSD1306_WIDTH, SSD1306_HEIGHT);

    while (1) {
        gpio_set_level(TRIG_PIN, 0);
        vTaskDelay(2 / portTICK_RATE_MS);

        gpio_set_level(TRIG_PIN, 1);
        vTaskDelay(10 / portTICK_RATE_MS);
        gpio_set_level(TRIG_PIN, 0);

        while (gpio_get_level(ECHO_PIN) == 0) {
            ;
        }
        uint32_t start_time = esp_log_timestamp();

        while (gpio_get_level(ECHO_PIN) == 1) {
            ;
        }
        uint32_t end_time = esp_log_timestamp();

        uint32_t duration = end_time - start_time;
        float distance = (float) duration * 0.0343 / 2;

        char distance_str[20];
        snprintf(distance_str, sizeof(distance_str), "Distance: %.2f cm", distance);

        ssd1306_clear();
        ssd1306_text(0, 0, distance_str, 16);
        ssd1306_refresh();

        ESP_LOGI(TAG, "%s", distance_str);
        vTaskDelay(500 / portTICK_RATE_MS);
    }
}

void app_main() {
    ESP_LOGI(TAG, "Ultrasonic Radar Started");
    xTaskCreate(ultrasonic_radar_task, "ultrasonic_radar_task", 2048, NULL, 5, NULL);
}
