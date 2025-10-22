# HueSyncStudio

HueSyncStudio is a professional, standalone Windows application for controlling Philips Hue Entertainment lights based on detailed, customizable audio analysis. The focus is on maximum precision, flexibility, and intuitive live control, with a particular emphasis on DJ and livestreaming scenarios.

**Note on Security:** The bundled `kissfft` library has been updated to the latest version via a Git submodule to resolve potential security issues found in the previously included local copy.

## Features

- **Multi-Bridge Support:** Stable connection to Hue Bridge v2 (REST + DTLS streaming), with support for multiple bridges and Entertainment groups.
- **Precise Audio Analysis:** Real-time FFT with dynamically adjustable frequency bands.
- **Dual-Mode Control:** Separate control for low-latency DTLS (audio sync) and standard-latency REST/HTTP (ambience/manual).
- **Visual Mapping:** Drag & Drop 2D mapping of lamp positions.
- **Manual Override:** Individual manual control of brightness/color per lamp or group.
- **Advanced Color Logic:** Support for color groups and color deviation.
- **Automation:** WebSocket server for external control.

## Key Technologies

| Module             | Library / Technology |
|--------------------|----------------------|
| GUI / UI           | Qt 6 + QML           |
| Audio Input        | RtAudio / WASAPI     |
| FFT                | KissFFT              |
| FFT                | KissFFT (submodule)  |
| Network & Hue API  | Qt Network           |
| Configuration/Data | nlohmann::json       |
| Testing            | Catch2               |
| Build System       | CMake                |
| CI/CD              | GitHub Actions       |
| Logging            | spdlog               |

## Building from Source

This project uses C++17, CMake, and the vcpkg dependency manager.

### Prerequisites

- A C++17 compliant compiler (e.g., MSVC, GCC, Clang).
- CMake (version 3.16 or higher).
- Git.

### Build Steps

1.  **Clone the repository:**
    ```bash
    git clone <repository-url>
    cd HueSyncStudio
    ```
1.  **Clone the repository and its submodules:**
    ```bash
    git clone --recursive <repository-url>
    cd HueSyncStudio
    ```
    If you have already cloned the repository without the `--recursive` flag, you can initialize the submodules with:
    ```bash
    git submodule update --init --recursive
    ```

2.  **Set up vcpkg:**
    The project is configured to use vcpkg for dependency management. Clone vcpkg into the project's root directory.
    ```bash
    git clone https://github.com/microsoft/vcpkg.git
    ./vcpkg/bootstrap-vcpkg.bat  # On Windows
    # ./vcpkg/bootstrap-vcpkg.sh # On Linux/macOS
    ```

3.  **Install dependencies:**
    The required dependencies will be installed automatically by CMake, but you can install them manually:
    ```bash
    # On Windows
    .\vcpkg\vcpkg.exe install qtbase qtnetwork qtwebsockets qtdeclarative spdlog nlohmann-json catch2 rtaudio --triplet x64-windows
    # On Linux
    ./vcpkg/vcpkg install qtbase qtnetwork qtwebsockets qtdeclarative spdlog nlohmann-json catch2 rtaudio --triplet x64-linux
    ```

4.  **Configure the project using CMake:**
    ```bash
    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake -DBUILD_TESTING=ON
    ```

5.  **Build the application:**
    ```bash
    cmake --build build --config Release
    ```
    The executable will be located in the `build/bin/Release` directory.

6.  **Run tests:**
    ```bash
    ctest --test-dir build --output-on-failure
    ```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
