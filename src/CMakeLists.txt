add_library(sdk_core
    sdk/gpio.cpp
    sdk/uart.cpp
    sdk/adc.cpp
)

add_library(cli
    shell/cli_manager.cpp
    shell/command_parser.cpp
    shell/history_manager.cpp
)

add_library(web_dashboard
    web/dashboard.cpp
)

target_include_directories(sdk_core
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(cli
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(web_dashboard
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Find required packages
find_package(nlohmann_json CONFIG REQUIRED PATHS ${CMAKE_BINARY_DIR})
find_package(Threads REQUIRED)
find_package(httplib CONFIG REQUIRED PATHS ${CMAKE_BINARY_DIR})
find_package(websocketpp CONFIG REQUIRED PATHS ${CMAKE_BINARY_DIR})
find_package(Boost CONFIG REQUIRED COMPONENTS system filesystem PATHS ${CMAKE_BINARY_DIR})

target_link_libraries(sdk_core
    PRIVATE
    nlohmann_json::nlohmann_json
    Threads::Threads
)

target_link_libraries(cli
    PRIVATE
    Threads::Threads
)

target_link_libraries(web_dashboard
    PRIVATE
    nlohmann_json::nlohmann_json
    Threads::Threads
    Boost::system
    Boost::filesystem
    websocketpp::websocketpp
    httplib::httplib
) 