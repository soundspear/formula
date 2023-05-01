/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#include "NoCompilerFoundPopup.hpp"

formula::gui::NoCompilerFoundPopup::NoCompilerFoundPopup()
{
    titleFont = title.getFont();
    titleFont.setHeight(20);
    title.setFont(titleFont);
    title.setText("No compiler found", NotificationType::sendNotification);
    title.setJustificationType(Justification::centred);
    topDescriptionLabel.setText("Mandatory dependencies are missing.\nYou can install them using the link below:",
                           NotificationType::sendNotification);
    topDescriptionLabel.setJustificationType(Justification::centred);

#if defined(_WIN32)
    downloadLink.setButtonText("Download LLVM compiler setup");
    downloadLink.setURL(juce::URL("https://github.com/llvm/llvm-project/releases/download/llvmorg-14.0.0/LLVM-14.0.0-win64.exe"));
    bottomDescriptionLabel.setText("Restart your DAW once installed", NotificationType::sendNotification);
#elif defined(__APPLE__)
    downloadLink.setButtonText("Install XCode from the App Store");
    downloadLink.setURL(juce::URL("https://apps.apple.com/app/xcode/id497799835"));
    bottomDescriptionLabel.setText("Restart your DAW once installed", NotificationType::sendNotification);
#elif defined(__linux__)
    downloadLink.setButtonText("Download LLVM");
    downloadLink.setURL(juce::URL("https://releases.llvm.org/download.html"));
    bottomDescriptionLabel.setText("Or install the latest 'clang' package using your package manager", NotificationType::sendNotification);
#endif

    bottomDescriptionLabel.setJustificationType(Justification::centred);

    addAndMakeVisible(title);
    addAndMakeVisible(topDescriptionLabel);
    addAndMakeVisible(downloadLink);
    addAndMakeVisible(bottomDescriptionLabel);
}

juce::Point<int> formula::gui::NoCompilerFoundPopup::getPopupSize() {
    return {400,175};
}

void formula::gui::NoCompilerFoundPopup::resized() {
    constexpr auto titleHeight = 50;
    constexpr auto topDescriptionHeight = 60;
    constexpr auto linkHeight = 30;
    constexpr auto bottomDescriptionHeight = 30;

    auto area = getLocalBounds();

    title.setBounds(area.removeFromTop(titleHeight));
    topDescriptionLabel.setBounds(area.removeFromTop(topDescriptionHeight));
    downloadLink.setBounds(area.removeFromTop(linkHeight));
    bottomDescriptionLabel.setBounds(area.removeFromTop(bottomDescriptionHeight));
}