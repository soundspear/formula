#include "SettingsTab.hpp"

formula::gui::SettingsTab::SettingsTab(
    const std::shared_ptr<formula::events::EventHub>& eventHubRef,
    const std::shared_ptr<formula::processor::FilePlayer>& filePlayerRef
)
    : eventHub(eventHubRef),
      filePlayer(filePlayerRef),
      settings(std::make_shared<formula::storage::LocalSettings>())
{
    windowSizeLabel.setText("Window size", juce::NotificationType::sendNotification);
    addAndMakeVisible(windowSizeLabel);
    setPossibleWindowSizes();
    windowSizeComboBox.addItemList(windowSizeList, 1);
    windowSizeComboBox.addListener(this);
    addAndMakeVisible(windowSizeComboBox);

    autoCompileLabel.setText("Auto-Launch", juce::NotificationType::sendNotification);
    addAndMakeVisible(autoCompileLabel);
    
    autoCompileToggle.setToggleState(settings->find<bool>(formula::storage::SettingKey::autoCompile).value_or(false), juce::NotificationType::dontSendNotification);
    autoCompileToggle.addListener(this);
    addAndMakeVisible(autoCompileToggle);

    autoCompileDelayLabel.setText("Auto-Launch Delay (ms)", juce::NotificationType::sendNotification);
    addAndMakeVisible(autoCompileDelayLabel);

    autoCompileDelayInput.setText(std::to_string(settings->find<int>(formula::storage::SettingKey::autoCompileDelay).value_or(100)));
    autoCompileDelayInput.setInputFilter(new juce::TextEditor::LengthAndCharacterRestriction(6, "0123456789"), false);
    autoCompileDelayInput.addListener(this);
    addAndMakeVisible(autoCompileDelayInput);
    
    // "Auto-Launch" Delay Validation
    auto autoCompileDelayInputValidation = [&]()
    {
        auto val = autoCompileDelayInput.getText().getIntValue();
        if (val < 100)
            autoCompileDelayInput.setText("100", juce::dontSendNotification);
    };
    autoCompileDelayInput.onFocusLost = autoCompileDelayInputValidation;
    autoCompileDelayInput.onReturnKey = autoCompileDelayInputValidation;

    autoCompileDelaySuffix.setText("ms", juce::NotificationType::sendNotification);
    addAndMakeVisible(autoCompileDelaySuffix);

    if (JUCEApplicationBase::isStandaloneApp()) {
        addAudioFileLoader();
    }
}

void formula::gui::SettingsTab::visibilityChanged() {
    repaint();
}

void formula::gui::SettingsTab::resized() {
    constexpr auto pad = 18;
    constexpr auto componentMargin = 8;

    auto area = getLocalBounds();

    auto col = area.withTrimmedBottom(pad).withTrimmedTop(pad).withTrimmedRight(pad).withTrimmedLeft(pad);
    windowSizeLabel.setBounds(col.removeFromTop(15));
    col.removeFromTop(componentMargin);
    windowSizeComboBox.setBounds(col.removeFromTop(24).removeFromLeft(area.getWidth() / 2));
    col.removeFromTop(componentMargin + pad);

    if (JUCEApplicationBase::isStandaloneApp())
    {
        col.removeFromTop(pad);
        auto row = col.removeFromTop(24);
        loadAudioFileButton.setBounds(row.removeFromLeft(area.getWidth() / 3));
        row.removeFromLeft(pad);
        audioFileName.setBounds(row);
        col.removeFromTop(componentMargin + pad);
    }

    // Auto Compile toggle button
    auto row = col.removeFromTop(24);
    autoCompileToggle.setBounds(row.removeFromLeft(area.getWidth() / 24));
    row.removeFromLeft(pad);
    autoCompileLabel.setBounds(row);
    col.removeFromTop(componentMargin);

    // Auto Compile delay input
    col.removeFromTop(componentMargin + pad);
    row = col.removeFromTop(24);
    autoCompileDelayInput.setBounds(row.removeFromLeft(area.getWidth() / 10));
    row.removeFromLeft(pad);
    autoCompileDelayLabel.setBounds(row);
}

void formula::gui::SettingsTab::setPossibleWindowSizes() {
    windowSizeList.add("650x350");
    windowSizeList.add("700x450");
    windowSizeList.add("900x450");
    windowSizeList.add("1200x700");
    windowSizeList.add("1600x900");
    windowSizeList.add("1920x1080");
}

void formula::gui::SettingsTab::addAudioFileLoader() {
    loadAudioFileButton.setButtonText("Load audio file");
    addAndMakeVisible(loadAudioFileButton);
    loadAudioFileButton.onClick = [this]() {
        auto chooserPath = File::getSpecialLocation(File::userHomeDirectory);
        auto& previousPath = this->filePlayer->getCurrentPath();
        if (!previousPath.isEmpty()) {
            chooserPath = juce::File(previousPath).getParentDirectory();
        }
        juce::FileChooser chooser("Select the file to load...",
            chooserPath, this->filePlayer->getWildcardForAllFormats(), true);

        auto fileChosen = chooser.browseForFileToOpen();

        if (!fileChosen) return;
        auto filePath = chooser.getResult().getFullPathName();
        this->filePlayer->loadFile(filePath);
        audioFileName.setText(chooser.getResult().getFileName(), juce::NotificationType::sendNotification);
    };
    audioFileName.setText("No audio file selected", juce::NotificationType::sendNotification);
    addAndMakeVisible(audioFileName);
}

void formula::gui::SettingsTab::comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged) {
    if (comboBoxThatHasChanged == &windowSizeComboBox) {
        eventHub->publish(EventType::windowSizeRequested, windowSizeComboBox.getText().toStdString());
    }
}

void formula::gui::SettingsTab::textEditorTextChanged(juce::TextEditor& textEditor) {
    if (&textEditor == &autoCompileDelayInput) {
        auto delayValue = textEditor.getText().getIntValue();
        if (delayValue < 100) delayValue = 100;
        settings->add<int>(formula::storage::SettingKey::autoCompileDelay, delayValue);
        eventHub->publish(EventType::autoCompileDelayChanged, delayValue);
    }
}

void formula::gui::SettingsTab::buttonClicked(juce::Button* button) {
    if (button == &autoCompileToggle) {
        auto isEnabled = autoCompileToggle.getToggleState();
        settings->add<bool>(formula::storage::SettingKey::autoCompile, isEnabled);
        eventHub->publish(EventType::autoCompileToggle, isEnabled);
    }
}
