cmake_minimum_required(VERSION 3.10)
project(ti_sdk_emulator VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Set CMake prefix path for Conan packages
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR}/generators)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/generators)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(tests)

# Find required packages
find_package(Threads REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(httplib CONFIG REQUIRED)
find_package(websocketpp CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED COMPONENTS system filesystem)

# Main executable
add_executable(ti_sdk_shell
    src/main.cpp
)

target_link_libraries(ti_sdk_shell
    PRIVATE
    sdk_core
    cli
    web_dashboard
    Threads::Threads
    nlohmann_json::nlohmann_json
    Boost::system
    Boost::filesystem
)

# Installation
install(TARGETS ti_sdk_shell
    RUNTIME DESTINATION bin
) 