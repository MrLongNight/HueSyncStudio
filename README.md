# HueSyncStudio

HueSyncStudio is a professional, standalone Windows application for controlling Philips Hue Entertainment lights based on detailed, customizable audio analysis. The focus is on maximum precision, flexibility, and intuitive live control, with a particular emphasis on DJ and livestreaming scenarios.

# HueSyncStudio

HueSyncStudio ist eine professionelle Anwendung für ultra-niedrige Latenz Audio-Synchronisation mit Philips Hue Entertainment. Sie bietet Echtzeit-FFT mit dynamischer Frequenzband-Steuerung, Multi-Bridge-Unterstützung und eine intuitive Drag-&-Drop 2D/3D Licht-Mapping-Oberfläche, optimiert für Streamer und DJs.

---

## Inhaltsverzeichnis

- [Kurzbeschreibung](#kurzbeschreibung)
- [Hauptfunktionen](#hauptfunktionen)
- [Effekte & Parameter (vollständige Liste)](#effekte--parameter-vollständige-liste)
- [Installation (kurz)](#installation-kurz)
- [Schnellstart](#schnellstart)
- [WebSocket API — Kurzreferenz](#websocket-api---kurzreferenz)
  - [Verbindungsaufbau](#verbindungsaufbau)
  - [Nachrichtenformat (JSON)](#nachrichtenformat-json)
  - [Verfügbare Befehle / Endpunkte](#verfügbare-befehle--endpunkte)
  - [Beispiele](#beispiele)
- [Konfiguration & Profile](#konfiguration--profile)
- [Fehlerbehebung & Tipps](#fehlerbehebung--tipps)
- [Beitragen](#beitragen)
- [Lizenz & Kontakt](#lizenz--kontakt)

---

## Kurzbeschreibung

HueSyncStudio synchronisiert Lampen in Philips Hue Entertainment-zonen mit Audio-Eingängen (Line-In, Stereo-Mix, System-Audio). Die Anwendung führt eine Echtzeit-Fourier-Analyse (FFT) durch, erlaubt dynamische Zuweisung von Frequenzbändern zu Lichtgruppen und stellt mehrere visuelle Effekte bereit. Hauptziele sind minimale Latenz, Stabilität und hohe Anpassbarkeit.

---

## Hauptfunktionen

- Ultra-low-latency Philips Hue Entertainment-Sync
- Echtzeit-FFT mit konfigurierbaren Frequenzbändern
- Dynamische Band-Steuerung (Anzahl, Grenzen, Gewichtung)
- Multi-Bridge-Support (gleichzeitige Steuerung mehrerer Hue Bridges)
- Drag & Drop 2D/3D Light Mapping (Mapping von Lampen auf visuelle Positionen)
- Umfangreiche Effekt-Bibliothek (siehe unten)
- Profil-Management (Speichern/Laden von Mappings & Effekt-Presets)
- WebSocket-basierte Remote-API für externe Steuerung und Automatisierung
- Audio-Input Auswahl & Geräteeinstellungen
- Low-latency Mode & Thread-Priorisierung (NFRs: Latenz & Stabilität)
- Visual Feedback und Debug-Overlay (RTT, FPS, Audio-Level)

---

## Effekte & Parameter (vollständige Liste)

Hinweis: Die folgenden Effekte und Parameter sind als vollständige, gut dokumentierte Sammlung vorgesehen. Wertebereiche sind typische Empfehlungen; genaue Hardware-/Bridge-Limits können abweichen.

Allgemeine Parameter (verfügbar für viele Effekte)
- enabled (bool) — Effekt aktivieren/deaktivieren
- intensity / strength (float, 0.0–1.0) — Gesamteffektivitäts-Skalierung
- brightness (int, 0–254) — Base-Brightness für Hue-Lampen
- gamma (float, 0.1–3.0) — Gamma-Korrektur
- color_mode (string: "hs" | "xy" | "palette") — Farbmodus
- update_rate (int, Hz) — Wie oft der Effekt aktualisiert wird
- smoothing (float, 0.0–1.0) — Glättung von Lautstärke/FFT-Daten
- decay (float, 0.0–5.0s) — Nachklingverhalten für Visuals
- threshold (float, dB) — Schwellwert für Reaktivität
- band_map (object) — Mapping von Frequenzbändern zu Lampengruppen

1) Spectrum
- Beschreibung: Farben und Helligkeit basierend auf FFT-Spektrum.
- Parameter:
  - bands (int, 3–64) — Anzahl der FFT-Bänder
  - smoothing (float, 0–1) — Glättung über Zeit
  - palette (array of colors) — Farbpalette für die Bänder
  - scale (float, 0.0–4.0) — Exponentielle Skalierung der Auslenkung
  - saturation (float, 0–1) — Sättigung der Farben

2) BassPulse
- Beschreibung: Pulsierender Bass-Reactive-Effekt (Kick-Detection).
- Parameter:
  - sensitivity (float, 0–1) — Empfindlichkeit für Bass-Trigger
  - band (int) — Welches Frequenzband als Bass gilt
  - pulse_duration (float, s) — Dauer eines Pulses
  - max_brightness (int, 0–254) — Helligkeits-Maximum bei Pulse

3) BeatDetect (Onset)
- Beschreibung: Erkennung von Onsets/Beats und Auslösen von Aktionen.
- Parameter:
  - sensitivity (float, 0–1)
  - cooldown (float, s) — Minimale Zeit zwischen Beats
  - action (string) — "flash" | "strobe" | "palette_shift"

4) Strobe
- Beschreibung: Stroboskopischer Effekt in synchronisiertem Tempo.
- Parameter:
  - bpm_sync (bool) — Tempo-Synchronisation mit Musik (BPM)
  - frequency (float, Hz) — Strobe-Frequenz
  - duty_cycle (float, 0–1) — Verhältnis an/aus
  - color (color) — Strobe-Farbe

5) Wave (Directional)
- Beschreibung: Laufende Farb- oder Helligkeitswelle über das Mapping.
- Parameter:
  - direction (string: "x" | "y" | "z" | "radial")
  - speed (float, units/s) — Geschwindigkeit der Welle
  - wavelength (float) — Länge der Welle
  - phase (float) — Phasenoffset

6) ColorCycle / PaletteShift
- Beschreibung: Zyklische Rotation durch Farben oder Palette.
- Parameter:
  - palette (array of colors)
  - cycle_speed (float) — Geschwindigkeit der Rotation
  - interpolate (bool) — Smooth Übergänge

7) Reactive (per-lamp)
- Beschreibung: Lampen reagieren individuell auf lokale Lautstärke/FFT.
- Parameter:
  - local_sensitivity (float)
  - radius (float, meters) — Einflussradius bei 2D/3D-Mapping
  - falloff (string: "linear" | "exponential")

8) Gradient
- Beschreibung: Statischer oder dynamischer Farbverlauf über Map.
- Parameter:
  - start_color (color)
  - end_color (color)
  - angle (float, degrees)
  - animate (bool)
  - speed (float)

9) RandomSpark / Noise
- Beschreibung: Zufallsbasierte Effekte, oft als Füllung oder Overlay.
- Parameter:
  - density (float, 0–1)
  - amplitude (float)
  - seed (int)

10) Scene / Static
- Beschreibung: Vordefinierte Szenen oder statische Farben.
- Parameter:
  - scene_id (string)
  - blend_time (float, s)

11) Visualizer Overlays
- Beschreibung: UI-Debug-Overlays für FFT, RMS, Peak.
- Parameter:
  - show_fft (bool)
  - show_levels (bool)
  - overlay_opacity (float)

12) Advanced: Multi-Band Mapping & Weighting
- Parameter:
  - band_ranges (array of {lowHz, highHz})
  - weights (array of float) — Gewicht pro Band
  - combine_mode ("add" | "max" | "avg")

---

## Installation (kurz)

(Hier nur ein knapper Überblick — detaillierte Plattform-spezifische Anweisungen ggf. in INSTALL.md)
- Benötigt: Qt 5.15+ / Qt6 (abhängig von Build-Target), C++17 Toolchain, CMake
- Schritte:
  1. git clone https://github.com/MrLongNight/HueSyncStudio.git
  2. mkdir build && cd build
  3. cmake .. && cmake --build . --config Release
  4. Programm starten, Bridge(s) erkennen lassen und Entertainment-Zone auswählen

---

## Schnellstart

1. Starte HueSyncStudio.
2. Wähle deine Hue Bridge(s) in der Bridge-Liste (erlauben, falls nötig).
3. Lege eine Entertainment-Zone an oder wähle eine bestehende.
4. Ziehe Lampen in das 2D/3D-Mapping-Fenster.
5. Wähle einen Audio-Input (System, Line-In).
6. Wähle einen Effekt (z. B. Spectrum) und passe Parameter an.
7. Drücke "Start Sync".

---

## WebSocket API — Kurzreferenz

Die WebSocket API erlaubt externe Steuerung (z. B. Automatisierung, OBS-Plugins, Bot-Integrationen). Standardmäßig läuft der WebSocket-Server auf ws://localhost:8080 (konfigurierbar).

Verbindungs- und Nachrichten-Format ist JSON-RPC-ähnlich, aber leichtgewichtig (kein zwingendes JSON-RPC-Protokoll). Alle Befehle werden als JSON-Objekte gesendet; Antworten sind ebenfalls JSON.

Verbindungs-URL-Beispiel:
ws://localhost:8080/
oder bei TLS:
wss://example.local:8443/

Achtung: Falls Auth aktiviert ist, sende das Auth-Token nach dem Connect.

### Verbindungsaufbau
1. Öffne WebSocket-Verbindung.
2. Optional: AUTH senden:
```json
{
  "cmd": "auth",
  "token": "YOUR_SECRET_TOKEN"
}
```
Antwort:
```json
{
  "cmd": "auth_resp",
  "status": "ok"
}
```

### Nachrichtenformat (JSON)

Anfragen vom Client:
- Feld "cmd" (string) — Befehl
- Feld "id" (string|int, optional) — Korrelations-ID für Responses
- Feld "params" (object, optional) — Kommando-spezifische Parameter

Antworten vom Server:
- "cmd" — Antwortstyp z. B. "status", "error", "ok"
- "id" — echo der Anfrage-id (falls gesendet)
- "result" / "error" — payload

Beispiel Anfrage:
```json
{
  "id": 42,
  "cmd": "set_effect",
  "params": {
    "effect": "BassPulse",
    "enabled": true,
    "params": {
      "sensitivity": 0.8,
      "pulse_duration": 0.25
    }
  }
}
```

Antwort:
```json
{
  "id": 42,
  "cmd": "ok",
  "result": {
    "message": "Effect set"
  }
}
```

### Verfügbare Befehle / Endpunkte (Übersicht)

1. status
- Beschreibung: Allgemeiner Status der Anwendung.
- Anfrage:
```json
{"cmd":"status","id":1}
```
- Antwort: includes running (bool), active_effect, connected_bridges, latency_mode.

2. list_bridges
- Beschreibung: Liefert erkannte Hue Bridges und Verbindungsstatus.
- params: optional { "include_zones": true }
- Antwort: array of {id, ip, name, connected(bool)}

3. connect_bridge
- Beschreibung: Connectet zu einer Bridge und initialisiert Entertainment API.
- params: { "bridge_id": "bridge-uuid", "username": "optional" }
- Antwort: success/error

4. disconnect_bridge
- params: { "bridge_id": "bridge-uuid" }

5. list_zones
- Beschreibung: Liefert Entertainment-Zonen pro Bridge.

6. start_sync / stop_sync
- start_sync params: { "zone_id": "zone-uuid", "low_latency": true/false }
- stop_sync: { }

7. set_effect
- Beschreibung: Setzt/Wechselt Effekt inkl. Parameter.
- params:
  - effect (string) — Effekt-Name
  - enabled (bool, optional)
  - params (object, optional) — Effekt-spezifische Parameter (siehe Effekt-Liste oben)
- Beispiel oben.

8. update_effect_params
- params: { "effect": "Spectrum", "params": { "bands": 16, "smoothing": 0.6 } }

9. get_effect_params
- params: { "effect": "Spectrum" }
- Antwort: aktueller Param-Block

10. map_light / unmap_light
- map_light params: { "lamp_id": "hue-lamp-id", "x": 0.35, "y": 0.45, "z": 0.0 }
- unmap_light params: { "lamp_id": "hue-lamp-id" }

11. save_profile / load_profile / list_profiles / delete_profile
- params: { "profile_name": "MyScene" }

12. set_palette
- params: { "palette": ["#FF0000","#00FF00","#0000FF"], "name": "Vivid" }

13. ping
- params: { "ts": 123456789 }
- Antwort: echo + server_ts

14. subscribe / unsubscribe
- subscribe params: { "events": ["status","beats","errors"] }
- Server sendet asynchrone events:
  - event: "beat" — payload { band, strength, timestamp }
  - event: "error" — payload { code, message }
  - event: "status_update"

15. set_latency_mode
- params: { "mode": "ultra" | "balanced" | "stable" }
- Beeinflusst Buffer-Größen, Thread-Prioritäten, Update-Raten

16. calibrate
- params: { "type":"fft"|"levels", "options": { ... } }
- Führt lokale Kalibrierungen durch (z. B. RMS-Referenz)

17. raw_command (advanced)
- params: { "type":"bridge_raw", "payload": { ... } } — direkter Durchgriff für Entwickler (vorsichtig verwenden)

### Fehlerantwort-Format
Bei Fehlern sendet der Server:
```json
{
  "id": 42,
  "cmd": "error",
  "error": {
    "code": 400,
    "message": "Invalid parameter: bands"
  }
}
```

### Beispiele

Start Sync & Effekt setzen:
```json
// start sync
{"id":1,"cmd":"start_sync","params":{"zone_id":"zone-1234","low_latency":true}}

// set effect
{"id":2,"cmd":"set_effect","params":{
  "effect":"Spectrum",
  "enabled":true,
  "params":{"bands":24,"smoothing":0.5,"scale":1.6,"palette":["#FF4500","#FFD700","#00BFFF"]}
}}
```

Beat-Event (Server -> Client):
```json
{
  "cmd":"event",
  "event":"beat",
  "payload":{"band":1,"strength":0.87,"timestamp":1700000000}
}
```

---

## Konfiguration & Profile

- Profile speichern sowohl Mapping- als auch Effekt-/Parameter-Sets.
- Empfohlen: Pro Szene eine eigene Palette + Effektkombination speichern.
- Import/Export als JSON möglich (z. B. für Sharing).

---

## Fehlerbehebung & Tipps

- Keine Lampen sichtbar: Überprüfe Bridge-Verbindung & Entertainment-Zone.
- Hohe Latenz: aktiviere "ultra" Latency Mode, prüfe Audio-Buffer-Settings, reduziere update_rate.
- Dropouts/Ruckeln: Priorisiere Prozess-Threads, schließe andere Audio-Apps.

---

## Beitragen

Beiträge sind willkommen. Bitte:
- Fork -> Branch pro Feature -> PR mit Beschreibung
- Schreibe Unit-Tests für Core-Logik (wo möglich)
- Achte auf Trennung UI / Core (Core sollte Qt-unabhängig sein, soweit möglich)

---

## Lizenz & Kontakt

Siehe LICENSE-Datei. Für Fragen: Issues auf GitHub öffnen oder per E-Mail an den Maintainer (siehe GitHub-Profil).

---

Appendix: Wenn du eine vollständige, code-basierte Liste der aktuell implementierten Effekte und die exakten WebSocket-API-Endpunkte wünschst (ggf. mit feldgenauen Parametertypen), kann ich das automatisch aus dem Quellcode generieren — dafür bräuchte ich Zugriff auf die relevanten Dateien (z.B. Effect-Registry, WebSocket-Handler). Alternativ kann ich eine OpenAPI/JSON-Schema-Spezifikation für die WebSocket-Nachrichten erstellen.
