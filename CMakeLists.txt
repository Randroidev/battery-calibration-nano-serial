cmake_minimum_required(VERSION 3.13)

# Initialize Pico SDK
# The path to the SDK can be set by defining PICO_SDK_PATH
# or by setting the PICO_SDK_PATH environment variable.
include(pico_sdk_import.cmake)

project(battery-calibration-pico C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the Pico SDK
pico_sdk_init()

# Add library subdirectory
add_subdirectory(lib/displaylib)

# Add executable and include directories
add_executable(${CMAKE_PROJECT_NAME}
    src/main.cpp
    src/battery_manager.cpp
    src/process_controller.cpp
    src/led_indicator.cpp
    src/st7789_driver.cpp
    src/st7735_driver.cpp
    src/display_manager.cpp
    src/input_manager.cpp
    src/config_manager.cpp
    src/font.cpp
    src/battery_reporter.cpp
    src/user_interface.cpp
    src/ui_manager.cpp
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE include)

# Link to Pico SDK libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    displaylib
    pico_stdlib
    hardware_gpio
    hardware_i2c
    hardware_spi
    hardware_timer
)

# Enable USB serial output
pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)
pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 0)

# Add other build options here
pico_add_extra_outputs(${CMAKE_PROJECT_NAME})
