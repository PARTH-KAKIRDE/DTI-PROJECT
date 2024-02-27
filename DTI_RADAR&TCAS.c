#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "ssd1306.h"

#define TRIGGER_PIN GPIO_NUM_13
#define ECHO_PIN GPIO_NUM_12

#define MOTOR_LEFT_PIN GPIO_NUM_4
#define MOTOR_RIGHT_PIN GPIO_NUM_5

#define MAX_DISTANCE 200 // Maximum distance for object detection in centimeters
#define TURN_THRESHOLD 50 // Threshold distance for initiating a turn
#define TURN_ANGLE 30 // Angle to turn when an object is detected

#define DEFAULT_VREF    1100        // Default voltage reference

static esp_adc_cal_characteristics_t *adc_chars;
static const adc_channel_t channel = ADC_CHANNEL_0; // GPIO36
static const adc_atten_t atten = ADC_ATTEN_DB_0;
static const adc_unit_t unit = ADC_UNIT_1;

void display_init() {
    i2c_master_init();
    ssd1306_init();
    ssd1306_clear_screen(0x00);
    ssd1306_contrast_control(0xCF);
}

void display_proximity(uint32_t distance) {
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "Distance: %d cm", distance);
    ssd1306_display_text(buffer, 0, 0, 1);
}

static void ultrasonic_sensor_init() {
    gpio_set_direction(TRIGGER_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);
}

static uint32_t ultrasonic_sensor_measure_distance() {
    gpio_set_level(TRIGGER_PIN, 0);
    vTaskDelay(2 / portTICK_RATE_MS);

    gpio_set_level(TRIGGER_PIN, 1);
    vTaskDelay(10 / portTICK_RATE_MS);
    gpio_set_level(TRIGGER_PIN, 0);

    while (gpio_get_level(ECHO_PIN) == 0) {}
    int64_t start = esp_timer_get_time();

    while (gpio_get_level(ECHO_PIN) == 1) {}
    int64_t end = esp_timer_get_time();

    return (end - start) / 58; // Divide by 58 to convert microseconds to centimeters
}

void motor_init() {
    gpio_set_direction(MOTOR_LEFT_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(MOTOR_RIGHT_PIN, GPIO_MODE_OUTPUT);
}

void stop_robot() {
    gpio_set_level(MOTOR_LEFT_PIN, 0);
    gpio_set_level(MOTOR_RIGHT_PIN, 0);
}

void move_forward() {
    // Adjust PWM duty cycle for forward motion
    gpio_set_level(MOTOR_LEFT_PIN, 1);
    gpio_set_level(MOTOR_RIGHT_PIN, 1);
}

void turn_right() {
    // Adjust PWM duty cycle for turning right
    gpio_set_level(MOTOR_LEFT_PIN, 1);
    gpio_set_level(MOTOR_RIGHT_PIN, 0);
}

void app_main() {
    display_init();
    ultrasonic_sensor_init();
    motor_init();

    while (1) {
        uint32_t distance = ultrasonic_sensor_measure_distance();
        printf("Distance: %d cm\n", distance);
        display_proximity(distance);

        if (distance < MAX_DISTANCE) {
            // Object detected
            if (distance < TURN_THRESHOLD) {
                // Turn the robot
                turn_right();
            } else {
                // Stop the robot
                stop_robot();
            }
        } else {
            // No object detected, move forward
            move_forward();
        }

        vTaskDelay(100 / portTICK_RATE_MS);
    }
}
