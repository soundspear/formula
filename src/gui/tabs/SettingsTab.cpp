#include "SettingsTab.hpp"

formula::gui::SettingsTab::SettingsTab(
    const std::shared_ptr<formula::events::EventHub>& eventHubRef,
    const std::shared_ptr<formula::processor::FilePlayer>& filePlayerRef
)
    : eventHub(eventHubRef),
      filePlayer(filePlayerRef)
{
    windowSizeLabel.setText("Window size", juce::NotificationType::sendNotification);
    addAndMakeVisible(windowSizeLabel);
    setPossibleWindowSizes();
    windowSizeComboBox.addItemList(windowSizeList, 1);
    windowSizeComboBox.addListener(this);
    addAndMakeVisible(windowSizeComboBox);

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

    auto row = col.removeFromTop(24);
    loadAudioFileButton.setBounds(row.removeFromLeft(area.getWidth() / 3));
    row.removeFromLeft(pad);
    audioFileName.setBounds(row);
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
