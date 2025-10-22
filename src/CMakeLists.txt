find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

set(CMAKE_AUTOMOC ON)

qt_add_qml_module(HueSyncStudio
    URI "HueSyncStudio.ui"
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
    core/AudioAnalyzer.cpp
    ui/AudioDataModel.cpp
    ui/LampModel.cpp
    core/HueBridge.cpp
    core/MultiBridgeManager.cpp
    core/LampGroupManager.cpp
    core/ColorGroupEditor.cpp
    core/EffectEngine.cpp
    core/effects/PulseEffect.cpp
    core/effects/SpectrumEffect.cpp
    core/effects/ColorFlowEffect.cpp
    core/PresetManager.cpp
    core/WebSocketServer.cpp
)

target_include_directories(HueSyncStudio PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)


target_link_libraries(HueSyncStudio PRIVATE
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    kissfft
    rtaudio
    Qt6::Network
    Qt6::WebSockets
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
)

# --- Installation Rules ---
install(TARGETS HueSyncStudio
    RUNTIME DESTINATION bin
    COMPONENT Applications
)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/resources/"
    DESTINATION bin/resources
    COMPONENT Applications
)
