# This file is required by CMake to find the Pico SDK.
# It is normally located in the project's root directory.

# Protect against multiple inclusion
if(PICO_SDK_IMPORTED)
    return()
endif()
set(PICO_SDK_IMPORTED 1)

# Check for PICO_SDK_PATH environment variable
if(NOT PICO_SDK_PATH)
    if(DEFINED ENV{PICO_SDK_PATH})
        set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
        message(STATUS "Using PICO_SDK_PATH from environment ('${PICO_SDK_PATH}')")
    else()
        message(FATAL_ERROR "PICO_SDK_PATH not set")
    endif()
endif()

# Check that the SDK path is valid
if(NOT EXISTS ${PICO_SDK_PATH}/pico_sdk_init.cmake)
    message(FATAL_ERROR "PICO_SDK_PATH does not point to a valid SDK.")
endif()

set(PICO_SDK_PATH ${PICO_SDK_PATH} CACHE PATH "Path to the Raspberry Pi Pico SDK")

include(${PICO_SDK_PATH}/pico_sdk_init.cmake)
