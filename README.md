# R2R AI ASSISTANT 2.0

A Windows-first JUCE VST3 music-production assistant with a dark interface, neon cyan/red accents, MIDI utilities, music theory tools, and an optional AI-service integration.

## Build

- Windows 10/11
- Visual Studio 2022 with Desktop development with C++
- CMake 3.22+
- JUCE 8 (fetched by CMake)

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target R2RAIAssistant_VST3
```

The VST3 bundle is produced under `build/R2R AI Assistant 2.0_artefacts/Release/VST3/`.

## AI connection

The first version ships with a safe integration boundary and local music utilities. No API key is embedded in the plugin. Configure a provider endpoint and key through a future secure settings flow before enabling remote requests.

## Status

Initial project scaffold. A successful Windows build is required before calling this a compiled VST3 release.