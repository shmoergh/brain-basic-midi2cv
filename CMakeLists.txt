cmake_minimum_required(VERSION 3.22)

# Include Pico SDK (from brain-sdk subdirectory)
include(brain-sdk/pico_sdk_import.cmake)

project(brain-default-midi2cv C CXX ASM)

# Initialize the Pico SDK
pico_sdk_init()

# Add brain-sdk which includes brain libraries
add_subdirectory(brain-sdk)

add_executable(brain-default-midi2cv main.cpp)

target_link_libraries(brain-default-midi2cv
	pico_stdlib
	brain-common
	brain-utils
	brain-ui
	brain-io)

pico_enable_stdio_usb(brain-default-midi2cv 1)
pico_enable_stdio_uart(brain-default-midi2cv 1)
pico_add_extra_outputs(brain-default-midi2cv)
