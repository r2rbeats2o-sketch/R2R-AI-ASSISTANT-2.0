# Development roadmap

## Included
- Windows-first JUCE VST3/Standalone project
- Resizable dark UI with neon cyan and red accents
- Assistant, Chord Lab, MIDI Workspace, and Mix Guide pages
- Local chord triad MIDI event generation
- MIDI and audio pass-through
- Gemini 3.8 Flash online assistant through Google's Interactions API
- Gemini API key storage in Windows Credential Manager
- Background-thread HTTPS requests using WinHTTP
- Windows GitHub Actions VST3 build workflow

## Next milestones
- Multi-turn chat history and conversation reset
- Streaming responses and cancel-request support
- Persistent preset and project settings
- Scale-aware chord progressions and editable MIDI pattern generation
- Audio file analysis and waveform view
- Automated tests for MIDI timing, state save/restore, and plugin validation

## Verification still required
- Confirm a successful Windows GitHub Actions build for the latest commit
- Test a valid Gemini API key from the plugin on Windows
- Verify the VST3 loads and responds correctly in supported DAWs
