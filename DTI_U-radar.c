#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define TRIG_PIN GPIO_NUM_4
#define ECHO_PIN GPIO_NUM_2

static const char *TAG = "ultrasonic_radar";

void ultrasonic_radar_task(void *pvParameters) {
    gpio_pad_select_gpio(TRIG_PIN);
    gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(ECHO_PIN);
    gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);

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

        ESP_LOGI(TAG, "Distance: %.2f cm", distance);
        vTaskDelay(500 / portTICK_RATE_MS);
    }
}

void app_main() {
    ESP_LOGI(TAG, "Ultrasonic Radar Started");
    xTaskCreate(ultrasonic_radar_task, "ultrasonic_radar_task", 2048, NULL, 5, NULL);
}
