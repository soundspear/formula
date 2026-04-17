#ifndef FORMULA_GUI_SETTINGSTAB_INCLUDED
#define FORMULA_GUI_SETTINGSTAB_INCLUDED

#include <JuceHeader.h>
#include <events/EventHub.hpp>
#include <processor/FilePlayer.hpp>
#include <storage/LocalSettings.hpp>

namespace formula::gui {
    /**
     * Application tab that displays application settings
     */
    class SettingsTab : public juce::Component, juce::ComboBox::Listener, juce::TextEditor::Listener, juce::Button::Listener {
    public:
        explicit SettingsTab(
            const std::shared_ptr<formula::events::EventHub>& eventHubRef,
            const std::shared_ptr<formula::processor::FilePlayer>& filePlayerRef
        );
        void visibilityChanged() override;
        void resized() override;
    private:
        void setPossibleWindowSizes();
        void addAudioFileLoader();
        void comboBoxChanged(ComboBox *comboBoxThatHasChanged) override;
        void textEditorTextChanged(TextEditor& textEditor) override;
        void buttonClicked(Button* button) override;

        std::shared_ptr<formula::events::EventHub> eventHub;
        std::shared_ptr<formula::processor::FilePlayer> filePlayer;
        std::shared_ptr<formula::storage::LocalSettings> settings;

        juce::Label windowSizeLabel;
        juce::ComboBox windowSizeComboBox;
        juce::StringArray windowSizeList;
        juce::TextButton loadAudioFileButton;
        juce::Label audioFileName;

        juce::Label autoCompileLabel;
        juce::ToggleButton autoCompileToggle;
        juce::Label autoCompileDelayLabel;
        juce::TextEditor autoCompileDelayInput;
        juce::Label autoCompileDelaySuffix;
    };
}


#endif //FORMULA_GUI_SETTINGSTAB_INCLUDED
