# UI-Optimierung für HueSyncStudio

Ziel: Eine optimierte, priorisierte und umsetzungsfertige Liste von UI-Verbesserungen mit klaren Umsetzungsinfos, Risiken und genaueren Aufwandsschätzungen (in Personentagen, PD). Diese Datei dient als Entscheidungsgrundlage — nach deiner Auswahl kann ich konkrete QML-Wireframes und Code-Stubs erzeugen.

Hinweis zur Architekturannahme
- Trennung beibehalten: QML = UI, C++ = Core/Business-Logic.
- Kommunikation QML <-> C++ über registrierte QObject-Instanzen oder QAbstractListModel.
- Async statt blockierend: lange Operationen in Worker-Threads / QtConcurrent.

Datum: 2025-10-22  
Autor: UI-Optimierungsvorschlag (für MrLongNight / HueSyncStudio)

---

Inhaltsübersicht
- Priorität A — Kritisch (Sichtbarkeit & Stabilität)
- Priorität B — Hoher Nutzer-Nutzen (Bedienbarkeit)
- Priorität C — Präzision & Feintuning
- Priorität D — Optik, Accessibility & Design-System
- Priorität E — Erweiterte Features (größerer Architekturaufwand)
- Performance, QA & Telemetrie (Cross-cutting)
- Empfohlene 4‑Wochen-Sprintplanung
- Nächste Schritte / Akzeptanzkriterien

---

Priorität A — Kritisch (höchste Impact/Effort-Ratio)
Diese Items sollten zuerst umgesetzt werden: großer UX‑Gewinn bei geringem technischen Risiko.

1. Gerätestatus-Widget (Bridge & Lights)
- Nutzen: Sofortiger Überblick über Verbindungszustand, Anzahl Leuchten, IP, Auth-Status → weniger Support-Fälle.
- Umsetzung (kurz): Neuer Header-Bereich in QML (HeaderStatus.qml). Core stellt BridgeStatus QObject mit Properties: state (enum), bridgeIp, lightCount, lastSeen, authValid; Core sendet Signals bei Zustandswechsel.
- Betroffene Komponenten/Dateien (Vorschlag): UI/HeaderStatus.qml, src/core/BridgeManager.{h,cpp}, src/core/BridgeStatusModel.{h,cpp}
- Aufwand: S — 1.0–1.5 PD
- Risiken/Hinweise: FSM definieren (disconnected → scanning → authenticating → ready); update-Frequenz throttlen (max 1/s).
- Akzeptanzkriterien: Header zeigt korrekte Bridge-IP + Status; Status wechselt konsistent bei Connect/Disconnect; Unit-Test für Model-Signale.

2. Verbesserte Fehler- & Reconnect-UI (Inline Banner + Diagnose)
- Nutzen: Verstehbare Fehler, Retry & Diagnose-Optionen reduzieren Frust.
- Umsetzung: Inline ErrorBanner.qml (nicht-modal). Diagnose-Button triggert Core-Diagnose-Service (ping bridge, check tokens, list lights). Ergebnis als structured message anzeigen.
- Dateien: UI/ErrorBanner.qml, src/core/NetworkDiag.{h,cpp}
- Aufwand: S — 1.0–2.0 PD
- Risiken: Keine automatischen Uploads ohne Opt-in; Texte klar formulieren.
- Akzeptanzkriterien: bei Disconnect erscheint Banner; Diagnose liefert verständliche Ergebnisse; Retry löst erneute Verbindung.

3. UI-Thread entlasten (Async/Non-blocking)
- Nutzen: Keine Freezes bei Bridge-Scan, Preset-Laden oder Log-IO.
- Umsetzung: Identifizieren blocking Calls; für Scan/IO use QtConcurrent::run / QThreadPool; QFutureWatcher → signal → update UI.
- Dateien: src/core/BridgeManager.*, PresetManager.*, ggf. Änderungen an existing synchronous APIs.
- Aufwand: S — 1.5–3.0 PD
- Risiken: Thread-safety beachten; prefer queued signal/slot connections.
- Akzeptanzkriterien: UI bleibt responsiv bei großen IO-Operationen (manuelles Test-Skript).

---

Priorität B — Hoher Nutzer-Nutzen (Bedienbarkeit & Wiederverwendbarkeit)

4. Presets & Favoriten (Scene-Bibliothek)
- Nutzen: Schnellzugriff erhöht Nutzerbindung.
- Umsetzung: PresetManager (Core) mit CRUD, persistiert lokal als JSON; QAbstractListModel für ListView; Favoriten-Flag; Export/Import (.json).
- Dateien: UI/PresetsView.qml, src/core/PresetManager.{h,cpp}, models/PresetModel.{h,cpp}
- Aufwand: M — 3.0–5.0 PD
- Risiken: Migrationspfad für künftige Formatänderungen; Konfliktlösung bei Import.
- Akzeptanzkriterien: Anlegen, Editieren, Favorisieren, Export/Import funktioniert; ListView performant bei 100+ Presets.

5. Profile / Mode-Switcher (Gaming / Movie / Music / Ambient)
- Nutzen: Schnelles Wechseln ganzer Konfigurationssätze.
- Umsetzung: Profile-Definition als JSON (name, intensity, latencyOffset, colorMode, enabledLights). Core applyProfile(profileId) API. UI: große Kacheln mit Preview/Apply.
- Dateien: UI/ProfileTiles.qml, src/core/ProfileManager.{h,cpp}
- Aufwand: M — 2.5–4.0 PD
- Risiken: Konflikt mit aktiver Scene (entscheide: profiles override scene oder vice versa).
- Akzeptanzkriterien: Profile anlegen, anwenden, revert funktioniert, Preview korrekt.

---

Priorität C — Präzision & Feintuning

6. Latenz- und Synchronisations-Slider mit Test-Loop
- Nutzen: Korrigiert A/V-Offset, sichtbarer Komfortgewinn.
- Umsetzung: UI Slider mit Presets + numeric input; Test-Loop startet kurzes visuellen Test (Core erzeugt Test-Frames). Slider ändert Core SyncEngine latencyOffset (ms).
- Dateien: UI/Settings/LatencyControl.qml, src/core/SyncEngine.{h,cpp}
- Aufwand: S — 1.5–3.0 PD
- Risiken: Messbarkeit schwierig; klare UX zeigen, was 100ms usw. bedeuten.
- Akzeptanzkriterien: Slider ändert beobachtbar die Sync-Latenz im Test-Loop.

7. Live-Preview (performante Vorschau)
- Nutzen: Auswahl ohne Aktivierung testen.
- Umsetzung: QML Canvas/Shader-basiertes Preview, lightweight SampleFrames vom Core oder Simulation in UI. Fallback-Mode mit reduziertem Quality-Mode.
- Dateien: UI/Preview.qml, src/core/PreviewService.{h,cpp}
- Aufwand: M — 4.0–7.0 PD
- Risiken: Performance auf schwächeren Geräten; GPU-Shaders bevorzugen.
- Akzeptanzkriterien: Preview startet <200ms, flüssig bei 30fps in Test-Setup.

8. Farbwähler & Pipette (Eyedropper)
- Nutzen: Präzise Farbwahl, Pipette aus Screen-Region.
- Umsetzung: QML ColorPicker (HSV), optional native Screen Capture per Plattform für Pipette (macOS: CGDisplay, Windows: Desktop Duplication API). Core ColorUtils für Konvertierungen (XY / CT / HSV).
- Dateien: UI/ColorPicker.qml, src/core/ColorUtils.{h,cpp}
- Aufwand: M — 4.0–6.0 PD (plattformabhängig)
- Risiken: Berechtigungs-Prompts; platform-specific code.

---

Priorität D — Optik, Accessibility & Design-System

9. Responsives Layout & HiDPI-Support
- Nutzen: Gute Darstellung auf allen Plattformen.
- Umsetzung: Refactor QML Layouts (anchors, Column/RowLayouts), Icons als SVG, font-scaling setting.
- Dateien: UI/* (refactor), assets/icons/*.svg
- Aufwand: S→M — 2.0–5.0 PD (abhängig vom Refactorumfang)
- Risiken: QA-Aufwand auf mehreren DPIs.

10. Kontrastmodus & Barrierefreiheit (a11y)
- Nutzen: Barrierefreiheit, bessere Lesbarkeit.
- Umsetzung: Themes (default/contrast/large), QAccessible properties, keyboard navigation und Fokusreihenfolge prüfen.
- Dateien: UI/themes/*.qml, accessibility notes
- Aufwand: S — 2.0–3.5 PD
- Risiken: Zusätzliche Testmatrix (Screenreader, keyboard-only).

11. Konsistente QML-Komponentenbibliothek (Design-System)
- Nutzen: Konsistente UI, schnellere Entwicklung.
- Umsetzung: Basis-Komponenten (AppButton, Card, Toggle, ListItem) mit Props und Story-Doku.
- Dateien: UI/components/{AppButton,Card,Toggle}.qml
- Aufwand: M — 3.0–6.0 PD
- Risiken: Initialer Aufwand amortisiert sich schnell.

---

Priorität E — Erweiterte Features (größerer Architekturaufwand)

12. Multi-Bridge / Multi-Room Management
- Nutzen: Profi-Users: mehrere Bridges zentral verwalten.
- Umsetzung: BridgeContext-Manager in Core (multi-instance), UI Bridge-Selector + Raum-Mapping.
- Dateien: src/core/BridgeContextManager.*, UI/BridgeManager.qml
- Aufwand: L — 8.0–20.0 PD
- Risiken: Komplexes State-Management; Regressionstests nötig.

13. Scheduler / Timeline UI
- Nutzen: Szenen zeitlich planen (z. B. Sonnenuntergang).
- Umsetzung: Calendar UI, Recurrence rules, persistence; Core scheduler service (local).
- Dateien: UI/Scheduler/*.qml, src/core/SchedulerService.{h,cpp}
- Aufwand: M — 5.0–10.0 PD
- Risiken: Zeitzonen/DST Edge-Cases.

14. Remote Web-UI / Mobile Companion
- Nutzen: Steuerung per Smartphone/Browser.
- Umsetzung: REST/Websocket API in Core, kleine SPA (optional).
- Dateien: src/core/ApiServer.*, web-ui/
- Aufwand: L — 10.0–30.0 PD
- Risiken: Auth, Security, Maintenance.

---

Performance, QA & Telemetrie (Cross-cutting)

15. Optimierung großer Listen (ListView reuse)
- Umsetzung: QML ListView mit optimierten Delegates, Loader für heavy components.
- Aufwand: S — 1.0–2.0 PD

16. In-App Feedback & Log-Report (opt-in)
- Umsetzung: LogCollector, optional attach last N lines + screenshot, explicit opt-in.
- Aufwand: M — 3.0–6.0 PD
- Risiken: Datenschutz / DSGVO -> opt-in, anonymization.

17. UI E2E-Tests & Component Tests
- Umsetzung: QTest for C++ units, Squish/other for QML flows; write smoke tests for setup, start/stop sync.
- Aufwand: M — 5.0–10.0 PD (initial suite)

---

Empfohlene 4‑Wochen-Iteration (konservativ)
Ziel: sichtbare UX-Verbesserung + stabile Basis

Sprint 1 (4 Wochen, 1 Entwickler)
- Must: Gerätestatus-Widget (1), Verbesserte Fehler-UI (2), UI-Thread Async (3), Presets-Grundfunktionalität (4 minimal).
- Geschätzter Aufwand: 12–20 PD (inkl. Tests & Review)

Sprint 2 (4 Wochen)
- Should: Onboarding (siehe Anmerkung weiter unten), Latenz-Slider, Profile Switcher, Live-Preview (Basis).
- Geschätzter Aufwand: 15–25 PD

Optional Sprint-Planung für Onboarding
- Onboarding / Erstkonfiguration (Wizard)
- Aufwand: M — 5.0–8.0 PD
- Umsetzung: Schritt-für-Schritt: 1) Bridge-Discovery, 2) Auth mit Push-Status, 3) Default-Preset auswählen, 4) Short Tutorial. Persist FirstRun flag.

---

Nächste Schritte (konkrete Optionen — wähle 3–5 Items)
Wenn du mir sagst, welche Items ich zuerst umsetzen soll, liefere ich pro Item:
- QML-Wireframes / Komponenten-Liste (Dateinamen + minimaler QML-Code-Sketch),
- Benötigte Core-API-Änderungen mit Klassennamen & Signaturen (C++ stubs),
- Task-Breakdown in Sub-Tasks mit PD-Angaben und Tests,
- Optional: Patch/PR (auf Anfrage).

---

Akzeptanzkriterien (kurz)
- Jeder Task: klare Acceptance Tests (manuell + automatisiert möglich),
- Keine Blocking-Operationen im UI-Thread,
- Core-API backward-compatible, wenn möglich,
- Telemetrie nur mit explizitem Opt-in.

---

Kontakt / Entscheidung
- Bitte wähle 3–5 Items (z. B. "1.1 Gerätestatus-Widget, 1.2 Fehler-UI, 6.1 UI-Thread Async, 2.1 Presets-Grundfunktion").
- Ich bereite daraufhin die QML-Komponenten + C++-Stubs vor und kann die Änderung als Commit/PR vorschlagen (auf dein GO hin).
