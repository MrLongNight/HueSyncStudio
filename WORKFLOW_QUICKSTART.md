# Kurzanleitung: Build and Release Workflow

## Schnellstart

### Problem behoben [OK]

Die GitHub Actions Build-Pipeline funktioniert jetzt korrekt:
- [OK] Keine PowerShell-Encoding-Fehler mehr
- [OK] Alle Unicode-Zeichen durch ASCII ersetzt
- [OK] CMakeLists.txt vollständig und korrekt
- [OK] Workflow validiert und getestet

## Verwendung

### 1. Manueller Build & Release

```bash
# In GitHub UI:
# Actions → "Build and Release" → "Run workflow" → Wähle Branch → "Run workflow"
```

### 2. Automatischer Release bei Tag

```bash
# Lokale Version taggen:
git tag -a v1.0.0 -m "Release Version 1.0.0"
git push origin v1.0.0

# GitHub Actions startet automatisch und erstellt Release
```

### 3. Build bei Push auf main

```bash
git push origin main
# Workflow läuft, aber erstellt KEINEN Release
```

## Release-Artefakte

Nach erfolgreichem Build enthält der Release:

- `HueSyncStudio-1.0.0-Win64.zip` - Komplettes Paket (EXE + alle DLLs)
- Optional: NSIS-Installer (falls CPack erfolgreich)

## Workflow-Status prüfen

1. Gehe zu: https://github.com/MrLongNight/HueSyncStudio/actions
2. Wähle den aktuellen Workflow-Lauf
3. Prüfe jeden Step auf Erfolg/Fehler

## Build-Zeiten

- **Erster Build** (ohne Cache): ~30-60 Minuten
- **Mit Cache**: ~10-20 Minuten

## Wichtige Änderungen

### CMakeLists.txt
```cmake
project(HueSyncStudio VERSION 1.0.0 LANGUAGES CXX)
```

### Workflow
- Dateiname: `.github/workflows/build-and-release.yml` (mit Bindestrich!)
- Unicode-Zeichen ersetzt: Haken-Symbol → `[OK]`, X-Symbol → `[ERROR]`
- Encoding: us-ascii (sicher für PowerShell)

## Bei Problemen

### Cache löschen
```bash
# In GitHub UI:
# Actions → "Build and Release" → "..." Menü → "Delete workflow runs"
```

### Logs prüfen
```bash
# In GitHub UI:
# Actions → Wähle fehlgeschlagenen Run → Klicke auf fehlgeschlagenen Step
```

## Version ändern

Um eine neue Version zu releaen:

1. Ändere Version in `CMakeLists.txt`:
   ```cmake
   project(HueSyncStudio VERSION 1.1.0 LANGUAGES CXX)
   ```

2. Committe die Änderung:
   ```bash
   git add CMakeLists.txt
   git commit -m "Bump version to 1.1.0"
   git push origin main
   ```

3. Erstelle Tag:
   ```bash
   git tag -a v1.1.0 -m "Release 1.1.0"
   git push origin v1.1.0
   ```

## Weitere Dokumentation

Siehe `WORKFLOW_FIX_DOCUMENTATION.md` für:
- Detaillierte Fehleranalyse
- Vollständige Workflow-Struktur
- Alle 18 Build-Steps erklärt
- Fehlerbehebungs-Tipps

## Support

Bei Fragen oder Problemen:
1. Prüfe GitHub Actions Logs
2. Siehe `WORKFLOW_FIX_DOCUMENTATION.md`
3. Erstelle ein Issue auf GitHub
