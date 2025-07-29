message(STATUS "Conan: Using CMakeDeps conandeps_legacy.cmake aggregator via include()")
message(STATUS "Conan: It is recommended to use explicit find_package() per dependency instead")

find_package(nlohmann_json)
find_package(GTest)
find_package(httplib)
find_package(websocketpp)
find_package(Boost)

set(CONANDEPS_LEGACY  nlohmann_json::nlohmann_json  gtest::gtest  httplib::httplib  websocketpp::websocketpp  boost::boost )