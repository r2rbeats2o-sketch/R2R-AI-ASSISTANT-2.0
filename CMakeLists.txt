cmake_minimum_required(VERSION 3.22)
project(R2RAIAssistant VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)
FetchContent_Declare(JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 8.0.6
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(R2RAIAssistant
  COMPANY_NAME "R2R Vision"
  BUNDLE_ID "com.r2rvision.aiassistant"
  PLUGIN_MANUFACTURER_CODE R2RV
  PLUGIN_CODE R2A2
  PRODUCT_NAME "R2R AI Assistant 2.0"
  DESCRIPTION "AI-assisted music production tools"
  FORMATS VST3 Standalone
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT TRUE
  NEEDS_MIDI_OUTPUT TRUE
  IS_MIDI_EFFECT FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS TRUE
  COPY_PLUGIN_AFTER_BUILD FALSE)

target_sources(R2RAIAssistant PRIVATE
  Source/PluginProcessor.cpp
  Source/PluginEditor.cpp
  Source/GeminiClient.cpp)

target_compile_definitions(R2RAIAssistant PUBLIC
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
  JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(R2RAIAssistant PRIVATE
  juce::juce_audio_utils
  juce::juce_audio_processors
  juce::juce_gui_extra
  PUBLIC juce::juce_recommended_config_flags
         juce::juce_recommended_lto_flags
         juce::juce_recommended_warning_flags)

# Generate the aggregate JUCE header included by the plugin source files.
juce_generate_juce_header(R2RAIAssistant)
if (WIN32)
  target_link_libraries(R2RAIAssistant PRIVATE winhttp advapi32)
endif()\n