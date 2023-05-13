cmake_minimum_required(VERSION 3.11)

project(Formula VERSION 1.1.3 LANGUAGES CXX)

# ------ Formula parameters
add_compile_definitions(FORMULA_VERSION="${CMAKE_PROJECT_VERSION}")
if(NOT FORMULA_CLOUD_URL)
    set(FORMULA_CLOUD_URL https://formula.soundspear.com)
endif()
add_compile_definitions(FORMULA_CLOUD_URL="${FORMULA_CLOUD_URL}")
if(FORMULA_LOCAL_ONLY)
    add_compile_definitions(FORMULA_LOCAL_ONLY)
endif()
set(FORMULA_COMPILE_FORMATS "AU" "VST3")
if(FORMULA_STANDALONE)
    list (APPEND FORMULA_COMPILE_FORMATS "Standalone")
endif()
# ------

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(BOOST_BIND_GLOBAL_PLACEHOLDERS)
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
	set(Boost_USE_STATIC_RUNTIME ON)
    set(Boost_USE_STATIC_LIBS ON)
ELSEIF (APPLE)
    set(VCPKG_LIBRARY_LINKAGE static CACHE STRING "VCPKG_Library_Linkage")
    set(Boost_USE_STATIC_LIBS ON)
ENDIF()

# DEPENDENCIES
find_package(Boost 1.74.0 COMPONENTS REQUIRED filesystem date_time)
include_directories(Formula ${Boost_INCLUDE_DIR} SYSTEM)

add_subdirectory(JUCE)

find_package(cpprestsdk CONFIG REQUIRED)
add_compile_definitions(_TURN_OFF_PLATFORM_STRING)

IF (FORMULA_VST2_SDK_DIR)
    juce_set_vst2_sdk_path(${FORMULA_VST2_SDK_DIR})
    list (APPEND FORMULA_COMPILE_FORMATS "VST")
ENDIF ()

include_directories(Formula ${CMAKE_CURRENT_SOURCE_DIR}/src)
juce_add_plugin(Formula
    ICON_BIG assets/formula_icon.png
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
        assets/build/libformula.h
        assets/build/FormulaBaseMono.c
        assets/build/FormulaBaseStereo.c
        assets/default_index.json
        assets/formula_cloud_tos.txt
        assets/logo.svg
        assets/icons/compile.svg
        assets/icons/mute.svg
        assets/icons/new.svg
        assets/icons/pause.svg
        assets/icons/save_local.svg
        assets/icons/search.svg
        assets/icons/show_debug.svg
        assets/icons/show_knobs.svg
        assets/icons/unmute.svg
        assets/icons/unpause.svg
        assets/icons/zoom_in.svg
        assets/icons/zoom_out.svg
)
IF (UNIX)
    set_target_properties(FormulaBinaryData PROPERTIES
        POSITION_INDEPENDENT_CODE TRUE)
ENDIF()

add_compile_definitions(JUCE_MODAL_LOOPS_PERMITTED)
add_compile_definitions(JUCE_USE_MP3AUDIOFORMAT)

juce_generate_juce_header(Formula)

target_sources(Formula
    PRIVATE
        "src/cloud/AuthenticatedClient.cpp"
        "src/cloud/FormulaCloudClient.cpp"
        "src/cloud/GithubClient.cpp"
        "src/compiler/TccWrapper.cpp"
        "src/compiler/ClangWrapper.cpp"
        "src/gui/PluginWindow.cpp"
        "src/gui/FormulaCodeTokenizer.cpp"
        "src/gui/tabs/CodeEditorTab.cpp"
        "src/gui/tabs/SavedFilesTab.cpp"
        "src/gui/tabs/OnlineFormulasTab.cpp"
        "src/gui/tabs/SettingsTab.cpp"
        "src/gui/components/popups/SaveLocalFormulaPopup.cpp"
        "src/gui/components/popups/LoginPopup.cpp"
        "src/gui/components/popups/NoCompilerFoundPopup.cpp"
        "src/gui/components/popups/SetUserNamePopup.cpp"
        "src/gui/components/SpinnerOverlay.cpp"
        "src/gui/components/FormulaCodeEditor.cpp"
        "src/gui/components/RatingComponent.cpp"
        "src/gui/components/FormulaDetailsPanel.cpp"
        "src/gui/components/SearchBar.cpp"
        "src/processor/FilePlayer.cpp"
        "src/processor/library/FormulaLoader.cpp"
        "src/processor/KnobsPanelListener.cpp"
        "src/processor/PluginState.cpp"
        "src/processor/PluginProcessor.cpp"
        "src/compiler/CompilerWrapper.cpp"
        "src/storage/LocalIndex.cpp"
        "src/storage/LocalSettings.cpp"
        "src/storage/CompilerStorage.cpp")

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
ELSEIF (UNIX)
    target_link_libraries(Formula PRIVATE
            OpenSSL::SSL)
ENDIF()
