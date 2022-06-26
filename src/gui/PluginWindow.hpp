/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_GUI_PLUGINWINDOW_INCLUDED
#define FORMULA_GUI_PLUGINWINDOW_INCLUDED

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <cloud/GithubClient.hpp>
#include <events/EventHub.hpp>
#include <processor/FilePlayer.hpp>
#include <processor/PluginProcessor.hpp>
#include <storage/LocalIndex.hpp>
#include <gui/tabs/CodeEditorTab.hpp>
#include <gui/tabs/SavedFilesTab.hpp>
#include <gui/tabs/OnlineFormulasTab.hpp>
#include <gui/tabs/SettingsTab.hpp>
#include <gui/components/popups/LoginPopup.hpp>
#include <gui/components/popups/NoCompilerFoundPopup.hpp>
#include <gui/components/popups/SetUserNamePopup.hpp>
#include <gui/components/SpinnerOverlay.hpp>
#include <gui/FormulaLookAndFeel.hpp>

namespace formula::gui 
{
    /**
     * Base window for the application
     */
    class PluginWindow : public juce::AudioProcessorEditor
    {
    public:
        explicit PluginWindow(
            formula::processor::PluginProcessor& processor, 
            const std::shared_ptr<formula::events::EventHub>& eventHubRef,
            const std::shared_ptr<formula::processor::PluginState>& pluginStateRef,
            const std::shared_ptr<formula::cloud::FormulaCloudClient>& cloudRef,
            const std::shared_ptr<formula::storage::LocalIndex>& localIndexRef,
            const std::shared_ptr<formula::storage::LocalSettings>& settingsRef,
            const std::shared_ptr<formula::processor::FilePlayer>& filePlayer
        );
        ~PluginWindow() override;

        void setupPopups();

        void paint(juce::Graphics&) override;
        void resized() override;
    private:
        void setWindowSizeFromResolutionString(std::optional<std::string> resolutionStr);

        formula::processor::PluginProcessor& associatedProcessor;
        std::shared_ptr<formula::events::EventHub> eventHub;
        std::shared_ptr<formula::processor::PluginState> pluginState;
        std::shared_ptr<formula::cloud::FormulaCloudClient> cloud;
        std::shared_ptr<formula::storage::LocalSettings> settings;
        std::shared_ptr<formula::processor::FilePlayer> filePlayer;
        formula::cloud::GithubClient github;

        std::unique_ptr<juce::Drawable> logoDrawable;
        juce::Rectangle<float> logoPos;
        juce::Label versionLabel;
        juce::Font versionFont;

        juce::TabbedComponent tabs;
        SpinnerOverlay spinner;
        static std::unique_ptr<juce::TooltipWindow> tooltipWindow;
        formula::gui::LoginPopup loginPopup;
        formula::gui::NoCompilerFoundPopup noCompilerFoundPopup;
        formula::gui::SetUserNamePopup setUserNamePopup;
        std::unique_ptr<formula::gui::FormulaLookAndFeel> laf;

        juce::TextButton loadAudioFileButton;

        float scaleFactor = 1;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginWindow)
    };
}

#endif // FORMULA_GUI_PLUGINWINDOW_INCLUDED