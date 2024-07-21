#include "SettingsTab.hpp"

formula::gui::SettingsTab::SettingsTab(
    const std::shared_ptr<formula::events::EventHub>& eventHubRef
)
: eventHub(eventHubRef)
{
    windowSizeLabel.setText("Window size", juce::NotificationType::sendNotification);
    addAndMakeVisible(windowSizeLabel);
    setPossibleWindowSizes();
    windowSizeComboBox.addItemList(windowSizeList, 1);
    windowSizeComboBox.addListener(this);
    addAndMakeVisible(windowSizeComboBox);
}

void formula::gui::SettingsTab::visibilityChanged() {
    repaint();
}

void formula::gui::SettingsTab::resized() {
    constexpr auto pad = 18;
    constexpr auto componentMargin = 8;

    auto area = getLocalBounds();
    auto colWidth = area.getWidth() / 3;

    const auto areaCenter = area.getCentre();

    auto col1 = area.removeFromLeft(colWidth);
    col1 = col1.withTrimmedBottom(pad).withTrimmedTop(pad).withTrimmedRight(pad).withTrimmedLeft(pad);

    auto col2 = area.removeFromLeft(colWidth);
    col2 = col2.withTrimmedBottom(pad).withTrimmedTop(pad).withTrimmedRight(pad).withTrimmedLeft(pad);
    windowSizeLabel.setBounds(col2.removeFromTop(15));
    col2.removeFromTop(componentMargin);
    windowSizeComboBox.setBounds(col2.removeFromTop(24));
}

void formula::gui::SettingsTab::setPossibleWindowSizes() {
    windowSizeList.add("650x350");
    windowSizeList.add("700x450");
    windowSizeList.add("900x450");
    windowSizeList.add("1200x700");
    windowSizeList.add("1600x900");
    windowSizeList.add("1920x1080");
}

void formula::gui::SettingsTab::comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged) {
    if (comboBoxThatHasChanged == &windowSizeComboBox) {
        eventHub->publish(EventType::windowSizeRequested, windowSizeComboBox.getText().toStdString());
    }
}
