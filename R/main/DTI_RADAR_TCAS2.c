#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "sra_board.h"

#define TRIGGER_PIN GPIO_NUM_13
#define ECHO_PIN GPIO_NUM_12

#define MAX_DISTANCE 30 // Maximum distance for object detection in centimeters

#define TURN_THRESHOLD_FRONT 20 // Threshold distance for initiating a front turn
#define TURN_THRESHOLD_LEFT 15  // Threshold distance for initiating a left turn
#define TURN_THRESHOLD_RIGHT 10 // Threshold distance for initiating a right turn

#define TURN_ANGLE_RIGHT 60 // Angle to turn when an object is detected on the right side
#define TURN_ANGLE_LEFT 60  // Angle to turn when an object is detected on the left side

static void ultrasonic_sensor_init() {
    gpio_set_direction(TRIGGER_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);
}

static uint32_t ultrasonic_sensor_measure_distance() {
    gpio_set_level(TRIGGER_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(2)); // Trigger pulse
    gpio_set_level(TRIGGER_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(10)); // 10us pulse
    gpio_set_level(TRIGGER_PIN, 0);

    // Measure echo time to calculate distance
    TickType_t start = xTaskGetTickCount();
    while (gpio_get_level(ECHO_PIN) == 0) {}
    TickType_t echo_start = xTaskGetTickCount();
    while (gpio_get_level(ECHO_PIN) == 1) {}
    TickType_t echo_end = xTaskGetTickCount();

    TickType_t echo_duration = echo_end - echo_start;
    uint32_t distance = (echo_duration * 340 * 100) / (2 * 1000); // Calculate distance in cm

    return distance;
}

void stop_robot() {
    // Assuming MOTOR_A_0 and MOTOR_A_1 are valid motor handles obtained from enable_motor_driver
    set_motor_speed(MOTOR_A_0, MOTOR_STOP, 0);
    set_motor_speed(MOTOR_A_1, MOTOR_STOP, 0);
}

void move_forward() {
    set_motor_speed(MOTOR_A_0, MOTOR_FORWARD, 50); // Maximum speed forward
    set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, 50); // Maximum speed forward
}

void turn_right() {
    set_motor_speed(MOTOR_A_0, MOTOR_FORWARD, 25);  // Reduce speed for turning
    set_motor_speed(MOTOR_A_1, MOTOR_BACKWARD, 25); // Reverse direction for turning
}

void turn_left() {
    set_motor_speed(MOTOR_A_0, MOTOR_BACKWARD, 25); // Reverse direction for turning
    set_motor_speed(MOTOR_A_1, MOTOR_FORWARD, 25);  // Reduce speed for turning
}

void app_main() {
    ultrasonic_sensor_init();

    while (1) {
        uint32_t distance = ultrasonic_sensor_measure_distance();
        printf("Distance: %ld cm\n", distance);

        if (distance < MAX_DISTANCE) {
            if (distance < TURN_THRESHOLD_LEFT) {
                turn_left();
                vTaskDelay(pdMS_TO_TICKS(TURN_ANGLE_LEFT));
            } else if (distance < TURN_THRESHOLD_RIGHT) {
                turn_right();
                vTaskDelay(pdMS_TO_TICKS(TURN_ANGLE_RIGHT));
            } else if (distance < TURN_THRESHOLD_FRONT) {
                turn_right();
                vTaskDelay(pdMS_TO_TICKS(TURN_ANGLE_RIGHT));
            }
        } else {
            move_forward();
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100 milliseconds
    }
}
