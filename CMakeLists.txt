cmake_minimum_required(VERSION 3.16)
project(LiveSampler)

set(CMAKE_CXX_STANDARD 20)
set(LIVESAMPLER_VERSION "0.1.0")
set(LIVESAMPLER_COMPANY_NAME "wolfink")
set(LIVESAMPLER_COMPANY_WEBSITE "github.com/wolfink/LiveSampler")

# include submodule libraries
add_subdirectory(extern)

file(GLOB LiveSamplerSources 
    src/*.cpp
    src/*.h
)

juce_add_plugin(LiveSampler
    PRODUCT_NAME       "LiveSampler"
    VERSION            ${LIVESAMPLER_VERSION}
    PLUGIN_DESCRIPTION "A sampler that takes in live input to use with MIDI" 
    IS_SYNTH           TRUE
    NEEDS_MIDI_INPUT   TRUE
    NEEDS_MIDI_OUTPUT  FALSE
    IS_MIDI_EFFECT     FALSE
    FORMATS            VST3
    VST3_CATEGORIES    Instrument)

juce_add_gui_app(LiveSampler_standalone
    PRODUCT_NAME       "LiveSampler"
    VERSION            ${LIVESAMPLER_VERSION}
)

juce_generate_juce_header(LiveSampler)
set_target_properties(LiveSampler PROPERTIES CXX_STANDARD 20)
target_sources(LiveSampler PRIVATE ${LiveSamplerSources})

juce_generate_juce_header(LiveSampler_standalone)
set_target_properties(LiveSampler_standalone PROPERTIES CXX_STANDARD 20)
target_sources(LiveSampler_standalone PRIVATE ${LiveSamplerSources})

source_group(TREE ${CMAKE_CURRENT_LIST_DIR} PREFIX "Source Files" FILES ${LiveSamplerSources})

set(libs
    juce::juce_audio_utils
    juce::juce_audio_plugin_client
    juce::juce_dsp)

set(SHARED_COMPILE_DEFINITIONS
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0)
set(STANDALONE_COMPILE_DEFINITIONS
    ${SHARED_COMPILE_DEFINITIONS})
set(PLUGIN_COMPILE_DEFINITIONS
    JUCE_VST3_CAN_REPLACE_VST2=0
    ${SHARED_COMPILE_DEFINITIONS}
    )

list(APPEND libs)

target_compile_definitions(LiveSampler PUBLIC ${PLUGIN_COMPILE_DEFINITIONS})
target_compile_definitions(LiveSampler_standalone PUBLIC ${STANDALONE_COMPILE_DEFINITIONS})

target_link_libraries(LiveSampler PRIVATE ${libs} juce::juce_recommended_config_flags)
target_link_libraries(LiveSampler_standalone PRIVATE ${libs} juce::juce_recommended_config_flags)