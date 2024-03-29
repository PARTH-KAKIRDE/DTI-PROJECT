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

#define ECHO_TIMEOUT_MS 2000 // 2 seconds timeout

static const char *TAG = "ultrasonic_radar";

void ultrasonic_radar_task(void *pvParameters) {
    gpio_pad_select_gpio(TRIG_PIN);
    gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(ECHO_PIN);
    gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);

    i2c_master_init();
    i2c_set_pin(I2C_NUM_0, I2C_SDA_PIN, I2C_SCL_PIN, I2C_MODE_MASTER);

    ssd1306_init();

    while (1) {
        gpio_set_level(TRIG_PIN, 0);
        vTaskDelay(2 / portTICK_RATE_MS);

        gpio_set_level(TRIG_PIN, 1);
        vTaskDelay(10 / portTICK_RATE_MS);
        gpio_set_level(TRIG_PIN, 0);

        // Wait for the echo pin to go HIGH
        TickType_t echo_start_tick = xTaskGetTickCount();
        while (gpio_get_level(ECHO_PIN) == 0) {
            if ((xTaskGetTickCount() - echo_start_tick) * portTICK_PERIOD_MS > ECHO_TIMEOUT_MS) {
                ESP_LOGE(TAG, "Timeout waiting for echo pin to go HIGH");
                break;
            }
        }

        uint32_t start_time = esp_log_timestamp();

        // Wait for the echo pin to go LOW
        TickType_t echo_end_tick = xTaskGetTickCount();
        while (gpio_get_level(ECHO_PIN) == 1) {
            if ((xTaskGetTickCount() - echo_end_tick) * portTICK_PERIOD_MS > ECHO_TIMEOUT_MS) {
                ESP_LOGE(TAG, "Timeout waiting for echo pin to go LOW");
                break;
            }
        }

        uint32_t end_time = esp_log_timestamp();
        uint32_t duration = end_time - start_time;
        float distance = (float)duration * 0.0343 / 2;

        char distance_str[20];
        snprintf(distance_str, sizeof(distance_str), "Distance: %.2f cm", distance);

        ssd1306_clear();
        ssd1306_draw_string(0, 0, distance_str);
        ssd1306_refresh();

        ESP_LOGI(TAG, "%s", distance_str);
        vTaskDelay(500 / portTICK_RATE_MS);
    }
}

void app_main() {
    ESP_LOGI(TAG, "Ultrasonic Radar Started");
    xTaskCreate(ultrasonic_radar_task, "ultrasonic_radar_task", 2048, NULL, 5, NULL);
}
