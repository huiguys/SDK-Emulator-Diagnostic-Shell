enable_testing()

# Find GTest package
find_package(GTest REQUIRED)
include(GoogleTest)

# Add test executable
add_executable(sdk_tests
    gpio_test.cpp
)

target_link_libraries(sdk_tests
    PRIVATE
    sdk_core
    GTest::GTest
    GTest::Main
)

target_include_directories(sdk_tests
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Register tests
gtest_discover_tests(sdk_tests) 