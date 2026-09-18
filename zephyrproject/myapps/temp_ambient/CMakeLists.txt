# Minimum CMake version
cmake_minimum_required(VERSION 3.20.0)

# Add additional modules
set(ZEPHYR_EXTRA_MODULES "${CMAKE_SOURCE_DIR}/../../modules/mcp9808")
#set(ZEPHYR_EXTRA_MODULES "${CMAKE_SOURCE_DIR}/../drivers/sensor/ti/opt3001")

# Locate the Zephyr RTOS source
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# Name the project
project(temp_ambient)

# Locate the source code for the application
target_sources(app PRIVATE src/main.c)