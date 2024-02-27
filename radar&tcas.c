#include <stdio.h>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"
#include "soc/uart_struct.h"
#include "ultrasonic.h"

#define TRIGGER_PIN GPIO_NUM_13
#define ECHO_PIN GPIO_NUM_12
#define MOTOR_A_PIN GPIO_NUM_26
#define MOTOR_B_PIN GPIO_NUM_27

#define DISTANCE_THRESHOLD 30

// Function prototypes
void moveForward();
void avoidCollision();

void app_main() {
    ultrasonic_sensor_t sensor = {
        .trigger_pin = TRIGGER_PIN,
        .echo_pin = ECHO_PIN
    };
    ultrasonic_init(&sensor);

    gpio_set_direction(MOTOR_A_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(MOTOR_B_PIN, GPIO_MODE_OUTPUT);

    while (1) {
        // Measure distance using ultrasonic sensor
        uint32_t distance = ultrasonic_measure_cm(&sensor);

        // If an object is detected within the threshold distance
        if (distance <= DISTANCE_THRESHOLD) {
            // Perform collision avoidance maneuver
            avoidCollision();
        } else {
            // Move forward
            moveForward();
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // Adjust delay based on your requirements
    }
}

void moveForward() {
    // Set motor pins to move the robot forward
    gpio_set_level(MOTOR_A_PIN, 1); // Set pin HIGH
    gpio_set_level(MOTOR_B_PIN, 1); // Set pin HIGH
}

void avoidCollision() {
    // Set motor pins to turn the robot
    gpio_set_level(MOTOR_A_PIN, 1); // Set pin HIGH
    gpio_set_level(MOTOR_B_PIN, 0); // Set pin LOW
    vTaskDelay(pdMS_TO_TICKS(1000)); // Adjust delay based on how much you want the robot to turn
}
