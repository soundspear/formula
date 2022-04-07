cmake_minimum_required(VERSION 3.11)

project(Formula VERSION 0.1 LANGUAGES CXX)

add_compile_definitions(FORMULA_LOCAL_ONLY)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

IF (WIN32)
    # Force static linking and static CRT for cpprestsdk
    add_compile_definitions(_NO_ASYNCRTIMP)
    set(VCPKG_TARGET_TRIPLET x64-windows-static)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)
    set(VCPKG_WIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-windows-static)
    IF (MSVC)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    ENDIF ()
ELSEIF (APPLE)
  #set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
  set(OPENSSL_ROOT_DIR /usr/local/opt/openssl/)
  find_package(OpenSSL REQUIRED)
  include_directories(Formula ${OPENSSL_INCLUDE_DIR})
ENDIF()

# DEPENDENCIES
add_subdirectory(JUCE)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost 1.74.0 COMPONENTS REQUIRED filesystem date_time)
include_directories(Formula ${Boost_INCLUDE_DIR} SYSTEM)


find_package(cpprestsdk CONFIG REQUIRED)

set(FORMULA_COMPILE_FORMATS "AU" "VST3" "Standalone")
IF (FORMULA_VST2_SDK_DIR)
    juce_set_vst2_sdk_path(${FORMULA_VST2_SDK_DIR})
    list (APPEND FORMULA_COMPILE_FORMATS "VST")
ENDIF ()

include_directories(Formula ${CMAKE_CURRENT_SOURCE_DIR}/src)
juce_add_plugin(Formula
    # ICON_BIG ..
    # ICON_SMALL ...
    COMPANY_NAME Soundspear
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Spea
    PLUGIN_CODE Form
    FORMATS "${FORMULA_COMPILE_FORMATS}"
    VST2_CATEGORY kPlugCategEffect
    VST3_CATEGORIES Fx Tools Network
    AU_MAIN_TYPE kAudioUnitType_Effect
    PRODUCT_NAME Formula)

juce_add_binary_data(FormulaBinaryData
    NAMESPACE formula::binary
    SOURCES
        assets/build/FormulaBaseMono.c
        assets/build/FormulaBaseStereo.c
        assets/icons/compile.svg
        assets/icons/mute.svg
        assets/icons/pause.svg
        assets/icons/save_local.svg
        assets/icons/show_knobs.svg        
        assets/icons/unmute.svg
        assets/icons/unpause.svg
        assets/icons/zoom_in.svg
        assets/icons/zoom_out.svg
)

add_compile_definitions(JUCE_MODAL_LOOPS_PERMITTED)
juce_generate_juce_header(Formula)

target_sources(Formula
    PRIVATE
        "src/gui/PluginWindow.cpp"
        "src/gui/tabs/CodeEditorTab.cpp"
        "src/gui/tabs/SavedFilesTab.cpp"
        "src/gui/tabs/OnlineFormulasTab.cpp"
        "src/gui/components/SaveLocalFormulaPopup.cpp"
        "src/gui/components/LoginPopup.cpp"
        "src/gui/components/SpinnerOverlay.cpp"
        "src/gui/components/RatingComponent.cpp"
        "src/gui/components/FormulaDetailsPanel.cpp"
        "src/processor/FormulaLoader.cpp"
        "src/processor/PluginState.cpp"
        "src/processor/PluginProcessor.cpp"
        "src/compiler/CompilerWrapper.cpp"
        "src/storage/LocalIndex.cpp"
        "src/storage/LocalSettings.cpp"
        "src/storage/CompilerStorage.cpp"
        "src/cloud/FormulaCloudClient.cpp"
        "src/cloud/AuthenticatedClient.cpp"
        )

target_compile_definitions(Formula
    PRIVATE
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(Formula
    PRIVATE
        FormulaBinaryData
        juce::juce_audio_utils
        Boost::filesystem
        Boost::date_time
        cpprestsdk::cpprest
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

IF (WIN32)
    target_link_libraries(Formula PRIVATE
            cpprestsdk::cpprestsdk_zlib_internal
            cpprestsdk::cpprestsdk_brotli_internal
            bcrypt.lib winhttp.lib crypt32.lib)
ELSEIF (APPLE)
  target_link_libraries(Formula PRIVATE ${OPENSSL_LIBRARIES})
ENDIF()