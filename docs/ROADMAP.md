# Development roadmap

## Included in the starter
- JUCE VST3/Standalone project structure for Windows
- Resizable dark UI with neon cyan and red accents
- Page navigation for Assistant, Chord Lab, MIDI Workspace, and Mix Guide
- Local chord triad MIDI event generation
- MIDI pass-through and audio pass-through
- GitHub Actions Windows build workflow

## Next implementation milestones
- Persistent preset and project settings
- Scale-aware chord progressions and editable MIDI pattern generation
- Audio file analysis and waveform view
- Secure AI provider settings using OS credential storage
- Remote assistant requests with explicit user opt-in and provider error handling
- Full automated tests for MIDI timing, state save/restore, and plugin validation

The current assistant panel is explicitly offline and does not pretend to contact an AI service.
