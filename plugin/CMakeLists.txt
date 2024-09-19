cmake_minimum_required(VERSION 3.22)

# Version is needed by JUCE.
project(CX11Synth VERSION 0.1.0)

# Adding a directory with the library/application name as a subfolder of the
# include folder is a good practice. It helps avoid name clashes later on.
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/CX11Synth")

# Adds a plugin target (that's basically what the Projucer does).
# https://github.com/juce-framework/JUCE/blob/master/docs/CMake%20API.md
juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME RyanSoft # change this
    IS_SYNTH TRUE # may change this
    IS_MIDI_EFFECT FALSE
    NEEDS_MIDI_INPUT TRUE # may change this
    NEEDS_MIDI_OUTPUT FALSE # may change this
    VST_NUM_MIDI_INS 16
    VST_NUM_MIDI_OUTS 16
    VST3_CATEGORIES Instrument Synth
    #VST3_COPY_DIR The location to which VST3 plugins will be copied after building if COPY_PLUGIN_AFTER_BUILD is set on this target. If you want to install all of the VST3 plugins in a subdirectory to a non-default location, you can set the JUCE_VST3_COPY_DIR property on the directory before adding the plugin targets, rather than setting this argument on each individual target.
    PLUGIN_MANUFACTURER_CODE RYST # change this
    PLUGIN_CODE EXPL # change this
    FORMATS Standalone VST3 # may change this
    PRODUCT_NAME "CX11Synth" # change this
)

# Sets the source files of the plugin project.
target_sources(${PROJECT_NAME}
    PRIVATE
        source/BinaryData.cpp
        source/Synth.cpp
        source/LookAndFeel.cpp
        source/RotaryKnob.cpp
        source/PluginEditor.cpp
        source/PluginProcessor.cpp
        #${INCLUDE_DIR}/Synth.h
        #${INCLUDE_DIR}/RotaryKnob.h
        ${INCLUDE_DIR}/PluginEditor.h
        ${INCLUDE_DIR}/PluginProcessor.h
)

# Sets the include directories of the plugin project.
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Links to all necessary dependencies. The present ones are recommended by JUCE.
# If you use one of the additional modules, like the DSP module, you need to specify it here.
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# These definitions are recommended by JUCE.
target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# Enables all warnings and treats warnings as errors.
# This needs to be set up only for your projects, not 3rd party
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /Wall /WX)
    # Silences some deprecation warnings that (hopefully) will be fixed in a future release of JUCE.
    # _CRT_SECURE_NO_WARNINGS is to silence strcpy vs strpy_s
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
            _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# In Visual Studio this command provides a nice grouping of source files in "filters".
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/..)
