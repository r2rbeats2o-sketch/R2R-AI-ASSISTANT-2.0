# R2R AI ASSISTANT 2.0

A Windows-first JUCE VST3 music-production assistant with a dark interface, neon cyan/red accents, MIDI utilities, music theory tools, and Gemini AI chat.

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

## Gemini AI connection

The AI Assistant page connects to Google's Gemini Developer API using the `gemini-3.8-flash` model and Google's Interactions REST API.

1. Create a Gemini API key in Google AI Studio: https://aistudio.google.com/app/apikey
2. Open R2R AI Assistant 2.0 in your DAW.
3. On the AI Assistant page, paste the key into the Gemini API Key field and choose **SAVE KEY**.
4. Enter a music-production question and choose **ASK GEMINI**.

The key is stored locally in the current Windows user's Credential Manager. It is not included in the source code or sent to this GitHub repository. The plugin sends the key to Google's Gemini API over HTTPS in the `x-goog-api-key` request header. Never share your API key or commit it to source control. Review your Google AI Studio project usage and applicable terms before using the API.

The current integration sends one prompt per request; multi-turn chat history, streaming output, and a hosted backend are not implemented yet. Gemini requests run on a background thread and use the Windows HTTPS networking stack. Network calls run on a background thread so the plugin's audio callback is not blocked.

## Status

Gemini client and UI integration are committed. A successful Windows GitHub Actions build and an actual API-key test are still required before calling this a verified working VST3 release.
