HueSyncStudio – Project-Instruction.md (Version 2.2)
Dieses Dokument dient als einzige, widerspruchsfreie Arbeitsgrundlage für die
Entwicklung der HueSyncStudio-Anwendung.

1. Projektübersicht
Zielsetzung
Entwicklung einer professionellen, eigenständigen Windows-App zur Steuerung von
Philips Hue Entertainment-Lampen, basierend auf detaillierter, anpassbarer
Audioanalyse. Der Fokus liegt auf maximaler Präzision, Flexibilität und
intuitiver Live-Steuerung (DJ Livestream-Fokus).

Kernanforderungen (Funktional)
 * Multi-Bridge Support: Stabile Verbindung zur Hue Bridge v2 (REST + DTLS-Streaming),
   Unterstützung für mehrere Bridges und Entertainment Gruppen.
 * Präzise Audioanalyse: Echtzeit-FFT mit dynamisch einstellbaren Frequenzbändern (TG11).
 * Dual-Mode Control: Getrennte Steuerung für Low-Latency DTLS (Audio-Sync) und
   Standard-Latency REST/HTTP (Ambience/Manuell).
 * Visuelles Mapping: Drag & Drop 2D-Mapping der Lampenpositionen (TG12).
 * Manuelle Override: Individuelle manuelle Steuerung von Helligkeit/Farbe pro
   Lampe/Gruppe.
 * Erweiterte Farblogik: Farbgruppen, Farbabweichung/Deviation.
 * Automatisierung: WebSocket-Server und Hotkey-Schnittstelle.

2. Schlüsseltechnologien
| Modul | Bibliothek / Technik | Anmerkung |
|---|---|---|
| GUI / UI | Qt 6 + QML | Moderne, modulare und responsive Benutzeroberfläche. |
| Audio Input | RtAudio / WASAPI | Stabile, native Audioaufnahme auf Windows. |
| FFT | KissFFT | Leichtgewichtige, zuverlässige Fast-Fourier-Transformation. |
| Netzwerk & Hue API | Qt Network | Basis für REST-Anfragen und PSK DTLS Streaming. |
| Konfiguration / Daten | nlohmann::json | JSON-Parsing für Konfigurationsdateien und Presets. |
| Testing | Catch2 | Unit-Tests für Core-Module. |
| Build-System | CMake | Cross-Plattform-Build-System (primär Windows-orientiert). |
| CI/CD | GitHub Actions | Automatisierter Build, Test, Package und Release-Prozess. |
| Logging | spdlog | Asynchrones Logging mit Log-Rotation. |

3. Projekt- & Modulstruktur
(Strukturdiagramm bleibt unverändert)

4. Workflow & Governance
(Regeln bleiben unverändert)

5. Non-Functional Requirements
(Tabelle bleibt unverändert)

6. CI/CD Pipeline (build_and_release.yml)
(Pipeline-Definition bleibt unverändert)

7. Task-Gruppen (TG) – Arbeitsanweisungen
​Jede TG ist eine abgeschlossene, selbständige Einheit.

TG-ID Modul Aufgabe und Ergebnis
TG01 Core & Projekt-Setup Erstelle die definierte Ordnerstruktur. Implementiere Logger
    (spdlog) und ConfigManager (nlohmann::json). Erstelle default_config.json.
    Verifiziere, dass Build und Tests in der CI-Umgebung fehlerfrei durchlaufen.
TG02 AudioAnalyse (Basis) Implementiere AudioAnalyzer mit RtAudio/WASAPI. Führe
    Basis-Echtzeit-FFT mit KissFFT durch. Extrahiere Frequenzbandwerte (Low / Mid / High).
TG03 HueBridge + Streaming Implementiere den MultiBridgeManager zur Verwaltung von
    mehreren Bridge-Instanzen. Jede Instanz muss Discovery/Authentifizierung (REST)
    und DTLS-Streaming (Entertainment API V2) unterstützen.
TG04 Lampensteuerung & Gruppenmanager Lade Lampen mit Positionen (x,y,z) von allen
    Bridges. Implementiere den LampGroupManager zur logischen Gruppierung. Adressierung
    von Steuerbefehlen pro Lampe/Gruppe über den korrekten Modus (DTLS oder REST).
TG05 ColorGroupEditor + Farbabweichung Implementiere die Logik des ColorGroupEditor
    zur Definition von Farbgruppen (Color Palettes). Implementiere den Parameter
    "Farbabweichung" (Deviation).
TG06 Modulare EffektEngine (Strategy) WICHTIG: Modularität durch Strategy Pattern
    sicherstellen. Implementiere eine abstrakte Basisklasse EffectBase.h mit einer
    rein virtuellen Methode `void applyEffect(...)`.
TG07 UI & Visualisierung (Modular) Ziel: Modernes, intuitives und anpassbares GUI.
    Architektur: AppWindow.qml ist der Hauptcontainer und muss ein Docking/Panel-System
    implementieren. Alle Hauptbereiche müssen über eine Menüleiste ausblendbar sein.
TG08 WebSocket-API Implementiere den WebSocketServer. Definiere und implementiere
    die Schnittstellen-Events zur externen Steuerung. Erweitere die Befehle, um
    die Ziel-Bridge und den Steuermodus explizit anzugeben.
TG09 Presets & Settings Implementiere das Speichern und Laden von Presets (Effekte,
    Gruppen, Farbgruppen, Bridge-Konfigurationen, etc.) als JSON.
TG10 Release & Installer Konfiguriere CPack/NSIS zur Erstellung eines Windows-Installers.
    Überprüfe den GitHub Release Prozess und die korrekte Erstellung der Artefakte.
TG11 Erweiterte Audioanalyse ERWEITERUNG: Füge die Möglichkeit hinzu, die Start- und
    Endfrequenz der Frequenzbänder in der Konfiguration zu definieren. Implementiere eine
    Peak-Hold / Decay-Logik.
TG12 Visuelles 2D/3D-Mapping ERWEITERUNG: Implementiere im LampMap.qml Drag & Drop,
    um die virtuelle x/y-Position der Lampen zu speichern. Füge eine optionale
    z-Koordinate hinzu.
