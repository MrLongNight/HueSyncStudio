HueSyncStudio – Project-Instruction.md (Version 2.2)
Dieses Dokument dient als einzige, widerspruchsfreie Arbeitsgrundlage für die Entwicklung der HueSyncStudio-Anwendung.

1. Projektübersicht
Zielsetzung
Entwicklung einer professionellen, eigenständigen Windows-App zur Steuerung von Philips Hue Entertainment-Lampen, basierend auf detaillierter, anpassbarer Audioanalyse. Der Fokus liegt auf maximaler Präzision, Flexibilität und intuitiver Live-Steuerung (DJ Livestream-Fokus).
Kernanforderungen (Funktional)
 * Multi-Bridge Support: Stabile Verbindung zur Hue Bridge v2 (REST + DTLS-Streaming), Unterstützung für mehrere Bridges und Entertainment Gruppen.
 * Präzise Audioanalyse: Echtzeit-FFT mit dynamisch einstellbaren Frequenzbändern (TG11).
 * Dual-Mode Control: Getrennte Steuerung für Low-Latency DTLS (Audio-Sync) und Standard-Latency REST/HTTP (Ambience/Manuell).
 * Visuelles Mapping: Drag & Drop 2D-Mapping der Lampenpositionen (TG12).
 * Manuelle Override: Individuelle manuelle Steuerung von Helligkeit/Farbe pro Lampe/Gruppe.
 * Erweiterte Farblogik: Farbgruppen, Farbabweichung/Deviation.
 * Automatisierung: WebSocket-Server und Hotkey-Schnittstelle.

2. Schlüsseltechnologien
| Modul | Bibliothek / Technik | Anmerkung |
|---|---|---|
| GUI / UI | Qt 6 + QML | Moderne, modulare und responsive Benutzeroberfläche. |
| Audio Input | RtAudio / WASAPI | Stabile, native Audioaufnahme auf Windows. |
| FFT | KissFFT | Leichtgewichtige, zuverlässige Fast-Fourier-Transformation. |
| Netzwerk & Hue API | libcurl + OpenSSL / Qt Network | Basis für REST-Anfragen (HTTP/HTTPS) und PSK DTLS Streaming. |
| Konfiguration / Daten | nlohmann::json | JSON-Parsing für Konfigurationsdateien und Presets. |
| Testing | Catch2 | Unit-Tests für Core-Module. |
| Build-System | CMake | Cross-Plattform-Build-System (primär Windows-orientiert). |
| CI/CD | GitHub Actions | Automatisierter Build, Test, Package und Release-Prozess. |
| Logging | spdlog | Asynchrones Logging mit Log-Rotation. |

3. Projekt- & Modulstruktur
HueSyncStudio/
├── CMakeLists.txt
├── Project-Instruction.md
├── README.md
├── LICENSE
│
├── src/
│   ├── main.cpp
│   ├── core/                  // Kernlogik, Headless-Betriebsfähigkeit
│   │   ├── Logger.h / Logger.cpp
│   │   ├── ConfigManager.h / .cpp
│   │   ├── AudioAnalyzer.h / .cpp  // RtAudio/KissFFT + Dynamische Bänder
│   │   ├── MultiBridgeManager.h / .cpp 
│   │   ├── HueBridge.h / .cpp      // REST & DTLS (pro Instanz)
│   │   ├── EffectEngine.h / .cpp   // Hauptsteuerlogik (DTLS + HTTP-Steuerung + Z-Achse)
│   │   ├── LampGroupManager.h / .cpp
│   │   ├── ColorGroupEditor.h / .cpp
│   │   └── WebSocketServer.h / .cpp
│   └── ui/                    // QML & C++-Integration
│       ├── AppWindow.qml
│       ├── AudioView.qml / AudioView.cpp // Interaktive FFT-Visualisierung
│       ├── LampMap.qml / LampMap.cpp     // Drag & Drop Mapping
│       ├── EffectPanel.qml / EffectPanel.cpp
│       └── ColorEditor.qml / ColorEditor.cpp
│
├── tests/
│   ├── CMakeLists.txt
│   ├── test_config.cpp
│   ├── test_audio.cpp
│   └── test_bridge.cpp
│
├── resources/
│   ├── default_config.json
│   ├── presets/
│   └── icons/
│
└── .github/
    └── workflows/
        └── build_and_release.yml

4. Workflow & Governance
​Branching-Regeln
​main: Stabile Release-Versionen. Commits nur über Pull Requests.
​dev: Aktuelle Entwicklungsbasis.
​Feature-Branches: feature/tgXX-description (z.B. feature/tg03-dtls-implementation).
​Commit-Namenschema
​Format: [TGxx] Kurze prägnante Beschreibung
​Beispiel: [TG01] Implementiere spdlog und konfiguriere default_config.json

​5. Non-Functional Requirements
NFR Anforderung Betroffene TGs
Latenz Gesamter Signalweg (Audio-Input bis Light-Update) NUR FÜR DTLS-STREAMING muss unter 40 ms liegen. TG02, TG03, TG06, TG11
Stabilität Bei Ausfall (Bridge nicht erreichbar, Audio-Device entfernt) darf die App nicht abstürzen. Fehler müssen sauber geloggt und in der UI gemeldet werden. TG02, TG03, TG07
Modularität UI-Module dürfen Core-Module (z.B. HueBridge) nur über definierte C++-Interfaces und QML-Bindings ansprechen. Direkter Zugriff ist verboten. Alle TGs
Testabdeckung Jede Core Task-Group (TG01-TG12) muss mindestens zwei Unit-Tests in Catch2 enthalten. Alle TGs (außer TG10)
Dual-Mode Control Die EffectEngine muss klar unterscheiden und priorisieren zwischen Low-Latency-DTLS und Standard-Latency-REST/HTTP.

6. CI/CD Pipeline (build_and_release.yml)
​Der Workflow startet manuell (workflow_dispatch) oder bei Push 
in main. Er führt Build, Tests, Packaging und Veröffentlichung der Release-Artefakte durch.

name: Build and Release

on:
  workflow_dispatch:
  push:
    branches: [ main ]

jobs:
  build:
    runs-on: windows-latest

    steps:
      - uses: actions/checkout@v4
      - name: Setup vcpkg
        run: |
          git clone [https://github.com/microsoft/vcpkg.git](https://github.com/microsoft/vcpkg.git) vcpkg
          .\\vcpkg\\bootstrap-vcpkg.bat
      - name: Install dependencies
        run: .\\vcpkg\\vcpkg.exe install qt6-base spdlog nlohmann-json catch2 rtaudio
      - name: Configure
        run: cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=${{ github.workspace }}\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake -DBUILD_TESTING=ON
      - name: Build
        run: cmake --build build --config Release
      - name: Run Tests
        run: ctest --test-dir build --output-on-failure
      - name: Package artifacts
        run: cmake --build build --config Release --target package || true
      - name: Upload Release Assets
        uses: softprops/action-gh-release@v2
        with:
          files: build/*.zip
          tag_name: v${{ github.run_number }}
          name: HueSyncStudio Release ${{ github.run_number }}

7. Task-Gruppen (TG) – Arbeitsanweisungen
Jede TG ist eine abgeschlossene, selbständige Einheit.
| TG-ID | Modul | Aufgabe und Ergebnis |
|---|---|---|
| TG01 | Core & Projekt-Setup | Erstelle die definierte Ordnerstruktur. Implementiere Logger (spdlog) und ConfigManager (nlohmann::json). Erstelle default_config.json. Verifiziere, dass Build und Tests in der CI-Umgebung fehlerfrei durchlaufen. |
| TG02 | AudioAnalyse (Basis) | Implementiere AudioAnalyzer mit RtAudio/WASAPI. Führe Basis-Echtzeit-FFT mit KissFFT durch. Extrahiere Frequenzbandwerte (Low / Mid / High). |
| TG03 | HueBridge + Streaming | Implementiere den MultiBridgeManager zur Verwaltung von mehreren Bridge-Instanzen. Jede Instanz muss Discovery/Authentifizierung (REST) und DTLS-Streaming (Entertainment API V2) unterstützen. |
| TG04 | Lampensteuerung & Gruppenmanager | Lade Lampen mit Positionen (x,y,z) von allen Bridges. Implementiere den LampGroupManager zur logischen Gruppierung. Adressierung von Steuerbefehlen pro Lampe/Gruppe über den korrekten Modus (DTLS oder REST). |
| TG05 | ColorGroupEditor + Farbabweichung | Implementiere die Logik des ColorGroupEditor zur Definition von Farbgruppen (Color Palettes). Implementiere den Parameter "Farbabweichung" (Deviation). |
| TG06 | EffektEngine | Implementiere die Hauptlogik der EffectEngine. Implementiere einen dedizierten Steuermodus für langsame/manuelle Effekte (REST/HTTP) und den DTLS-Modus. Erstelle Basis-Effekte (Pulse, Spectrum, ColorFlow) und eine manuelle Override-Funktion. Die Engine muss die optionalen Z-Werte (TG12) zur Effektberechnung einbeziehen. |
| TG07 | UI & Visualisierung | Erstelle das Haupt-QML-Interface (AppWindow.qml). Implementiere die Live-Visualisierung des FFT-Graphen (AudioView.qml) und die Live-Anzeige der Lampenkarte mit aktuellen Farben/Zuständen. Implementiere UI-Elemente zur Konfiguration mehrerer Bridges und zur manuellen Steuerung der Ambiance-Gruppen. |
| TG08 | WebSocket-API | Implementiere den WebSocketServer. Definiere und implementiere die Schnittstellen-Events zur externen Steuerung. Erweitere die Befehle, um die Ziel-Bridge und den Steuermodus explizit anzugeben. |
| TG09 | Presets & Settings | Implementiere das Speichern und Laden von Presets (Effekte, Gruppen, Farbgruppen, Bridge-Konfigurationen, sowie TG11/TG12-Daten) als JSON. Erstelle das zugehörige UI zur Verwaltung der Presets. |
| TG10 | Release & Installer | Konfiguriere CPack/NSIS zur Erstellung eines Windows-Installers. Überprüfe den GitHub Release Prozess und die korrekte Erstellung der Artefakte. |
| TG11 | Erweiterte Audioanalyse | ERWEITERUNG TG02/06: Füge die Möglichkeit hinzu, die Start- und Endfrequenz der Frequenzbänder (Low/Mid/High) in der Konfiguration zu definieren und zu speichern. Implementiere eine Peak-Hold / Decay-Logik in AudioAnalyzer zur Glättung der Peaks für weichere Lichteffekte. Aktualisiere AudioView.qml zur interaktiven Hervorhebung der konfigurierten Frequenzbänder auf dem FFT-Graphen. |
| TG12 | Visuelles 2D/3D-Mapping | ERWEITERUNG TG04/07: Implementiere im LampMap.qml die Funktion Drag & Drop der Lampen-Visualisierungen, um die virtuelle x/y-Position der Lampen zu speichern. Füge eine optionale z-Koordinate (Höhe) in die Lampen-Konfiguration (TG04/TG09) ein, um vertikale Effekte zu ermöglichen. Implementiere eine Gruppen-Mapping-Vorschau, die nur die aktuell ausgewählten Lampen hervorhebt. |


