cmake_minimum_required(VERSION 3.5)

# Set project name
project(DTI-PROJECT)

# Add FreeRTOS component
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(DTI-PROJECT)

# Add ESP32 component
idf_component_register(SRCS "DTI_RADAR&TCAS.c"
                    INCLUDE_DIRS "")

# Link FreeRTOS
target_link_libraries(${COMPONENT_LIB} INTERFACE freertos)

# Link ESP32 components
target_link_libraries(${COMPONENT_LIB} INTERFACE driver esp_adc_cal)

# Include ESP-IDF framework
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-function -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function -Wno-unused-parameter")

# Set additional compiler flags
target_compile_options(${COMPONENT_LIB} PRIVATE
    -std=gnu11
    -Wall
    -Werror
    -Wno-error=unused-variable
)

# Add linker script
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${PROJECT_PATH}/esp32_out.ld")

