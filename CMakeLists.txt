cmake_minimum_required(VERSION 3.16)
project(HueSyncStudio VERSION 1.0.0 LANGUAGES CXX)

# --- Build Improvements ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address,undefined)
    add_link_options(-fsanitize=address,undefined)
endif()

option(ENABLE_LINT "Enable clang-tidy" ON)
if(ENABLE_LINT)
    find_program(CLANGTIDY_EXE clang-tidy)
    if(CLANGTIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY "${CLANGTIDY_EXE};-warnings-as-errors=*")
    else()
        message(WARNING "clang-tidy not found. Linting disabled.")
    endif()
endif()

# --- Dependencies and Subdirectories ---
# Use CONFIG mode so CMake finds spdlogConfig.cmake provided by vcpkg
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)

add_subdirectory(libs)
add_subdirectory(src)

enable_testing()
add_subdirectory(tests)

# --- CPack/NSIS Installer Configuration ---
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "HueSyncStudio - Audio-reactive control for Philips Hue")
set(CPACK_PACKAGE_VENDOR "HueSyncStudio Project")
set(CPACK_PACKAGE_CONTACT "support@huesync.studio")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION}")
set(CPACK_NSIS_INSTALLER_ICON_CODE "")
set(CPACK_NSIS_CREATE_ICONS_EXTRA "CREATE_DESKTOP_LINK")
set(CPACK_NSIS_DISPLAY_NAME "${PROJECT_NAME}")
set(CPACK_NSIS_HELP_LINK "https://github.com/huesyncstudio-project/HueSyncStudio")
set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/huesyncstudio-project/HueSyncStudio")

include(CPack)