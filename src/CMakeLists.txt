find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

set(CMAKE_AUTOMOC ON)

qt_add_qml_module(HueSyncStudio
    URI "HueSyncStudio"
    VERSION "1.0"
    QML_FILES
        ui/AppWindow.qml
        ui/AudioView.qml
        ui/EffectPanel.qml
        ui/LampMap.qml
)

add_executable(HueSyncStudio
    main.cpp
    core/Logger.cpp
    core/ConfigManager.cpp
)

target_include_directories(HueSyncStudio PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/core"
)

target_include_directories(HueSyncStudio PUBLIC
    "${CMAKE_SOURCE_DIR}/libs/kissfft"
)

target_link_libraries(HueSyncStudio PRIVATE
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    kissfft
    rtaudio
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
)
