cmake_minimum_required(VERSION 3.21)

project(HueSyncStudio VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find vcpkg packages
find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(RtAudio CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Network WebSockets)

# Include subdirectories
add_subdirectory(libs)
add_subdirectory(src)

# Testing
option(BUILD_TESTING "Build tests" ON)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# --- CPack/NSIS Installer Configuration ---
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "HueSyncStudio - Audio-reactive control for Philips Hue")
set(CPACK_PACKAGE_VENDOR "HueSyncStudio Project")
set(CPACK_PACKAGE_CONTACT "support@huesync.studio")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

# NSIS specific settings
set(CPACK_GENERATOR "NSIS")
set(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION}")
set(CPACK_NSIS_DISPLAY_NAME "${PROJECT_NAME}")
set(CPACK_NSIS_HELP_LINK "https://github.com/MrLongNight/HueSyncStudio")
set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/MrLongNight/HueSyncStudio")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\HueSyncStudio.exe")
set(CPACK_NSIS_CREATE_ICONS_EXTRA
    "CreateShortCut '$DESKTOP\\\\HueSyncStudio.lnk' '$INSTDIR\\\\bin\\\\HueSyncStudio.exe'"
)
set(CPACK_NSIS_DELETE_ICONS_EXTRA
    "Delete '$DESKTOP\\\\HueSyncStudio.lnk'"
)

# Installer Branding
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/resources/icons/app_icon.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/resources/icons/app_icon.ico")

# Windows Start Menu
set(CPACK_NSIS_MENU_LINKS
    "bin/HueSyncStudio.exe" "HueSyncStudio"
    "https://github.com/MrLongNight/HueSyncStudio" "GitHub Repository"
)

include(CPack)
