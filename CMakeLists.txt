# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico-acquisition C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

file(GLOB SOURCES "src/*.c")
add_executable(pico-acquisition ${SOURCES})

pico_generate_pio_header(pico-acquisition ${CMAKE_CURRENT_LIST_DIR}/pio/reader.pio)
pico_generate_pio_header(pico-acquisition ${CMAKE_CURRENT_LIST_DIR}/pio/spi_slave.pio)

target_include_directories(pico-acquisition PRIVATE include)
pico_enable_stdio_usb(pico-acquisition 1)

target_link_libraries(pico-acquisition
        pico_stdlib hardware_pio hardware_dma hardware_irq hardware_clocks hardware_pwm)

pico_add_extra_outputs(pico-acquisition)
