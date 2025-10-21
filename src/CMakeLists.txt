add_executable(HueSyncStudio
    main.cpp
    core/Logger.cpp
    core/ConfigManager.cpp
    core/AudioAnalyzer.cpp
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
)
