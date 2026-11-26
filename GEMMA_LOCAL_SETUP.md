# Gemma Local for R2R AI Assistant 2.0 (Windows)

The VST3 can now use a local Gemma model through the Ollama HTTP API. The plugin sends requests to `http://127.0.0.1:11434/api/chat`; no Gemini API key is used in Gemma Local mode.

## Setup

1. Install Ollama for Windows from the official site: https://ollama.com/download
2. Open Windows Terminal or Command Prompt.
3. Download and start the default Gemma 4 E2B model:

   ```powershell
   ollama run gemma4:e2b
   ```

   Wait for the model download to finish. Ollama serves its local API on port 11434.
4. Keep Ollama running. Open R2R AI Assistant 2.0 in your DAW.
5. In **AI ASSISTANT**, choose **GEMMA LOCAL / OFFLINE**. The model field defaults to `gemma4:e2b`.
6. Enter a music-production prompt and press **ASK GEMMA**.

## Hardware notes

Gemma is available in multiple sizes. The E2B variant is the small starting point for laptops and modest PCs; larger variants need more memory and compute. Actual speed depends on available RAM, GPU, and model quantization. If the model is too slow or does not load, use a smaller compatible Gemma model and enter its exact Ollama model tag in the plugin.

## Privacy and network behavior

- In Gemma Local mode, the prompt is sent to the local Ollama service on the same PC, not to Google's Gemini API.
- The model must be downloaded once; after that, inference can run without internet.
- Gemini Online remains available as a separate provider and still requires a Gemini API key.
- This implementation uses WinHTTP and does not run network requests on the audio-processing thread.

## Troubleshooting

- **Cannot connect to Ollama:** Start Ollama from the Windows Start menu and retry.
- **Model not found:** Run `ollama run gemma4:e2b` once in Terminal, or change the plugin model field to an installed tag.
- **Slow response:** Close other memory-heavy apps or select a smaller model.
