cmake_minimum_required(VERSION 3.13)

# Always include it
include(pico_sdk_import.cmake)

# Project's name
project(reflex_test)

# SDK Initialization - Mandatory
pico_sdk_init()

# C/C++ project files
add_executable(reflex_test
    reflex_test.c
)

# pico_stdlib library. You can add more if they are needed
target_link_libraries(reflex_test pico_stdlib)

# Enable usb output, disable uart output
pico_enable_stdio_usb(reflex_test 1)
pico_enable_stdio_uart(reflex_test 0)

# Need to generate UF2 file for upload to RP2040
pico_add_extra_outputs(reflex_test)