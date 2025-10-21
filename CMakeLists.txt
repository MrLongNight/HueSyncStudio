cmake_minimum_required(VERSION 3.16)
project(HueSyncStudio VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 COMPONENTS Core Network REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)

add_subdirectory(libs)
add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
