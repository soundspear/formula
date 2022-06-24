/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#include "SetUserNamePopup.hpp"

formula::gui::SetUserNamePopup::SetUserNamePopup(const std::shared_ptr<formula::cloud::FormulaCloudClient>& cloudRef)
: cloud(cloudRef) {
    auto labelFont = titleLabel.getFont();
    labelFont.setHeight(20);
    titleLabel.setFont(labelFont);
    addAndMakeVisible(titleLabel);

    descriptionLabel.setText("Please choose an username", NotificationType::sendNotification);
    addAndMakeVisible(descriptionLabel);

    addAndMakeVisible(nameEditor);

    okButton.onClick = [this] {
        this->setVisible(false);
        cloud->setUsername(nameEditor.getText().toStdString());
    };
    okButton.setButtonText("Ok");
    addAndMakeVisible(okButton);
}

void formula::gui::SetUserNamePopup::setUserNameAlreadyExists(bool userNameAlreadyExists) {
    if (userNameAlreadyExists) {
        titleLabel.setText("Username is already taken!", NotificationType::sendNotification);
    } else {
        titleLabel.setText("Welcome to Formula Cloud!", NotificationType::sendNotification);
    }
}

juce::Point<int> formula::gui::SetUserNamePopup::getPopupSize() {
    return { 300, 175 };
}

void formula::gui::SetUserNamePopup::resized() {
    constexpr auto pad = 12;
    constexpr auto componentsMargin = 12;
    constexpr auto titleHeight = 22;
    constexpr auto descriptionHeight = 18;
    constexpr auto editorHeight = 24;
    constexpr auto buttonHeight = 24;
    constexpr auto buttonMarginX = 64;

    auto area = getLocalBounds();
    area = area.withTrimmedTop(pad).withTrimmedBottom(pad).withTrimmedLeft(pad).withTrimmedRight(pad);

    titleLabel.setBounds(area.removeFromTop(titleHeight));
    area.removeFromTop(componentsMargin);

    descriptionLabel.setBounds(area.removeFromTop(descriptionHeight));
    area.removeFromTop(componentsMargin);

    nameEditor.setBounds(area.removeFromTop(editorHeight));
    area.removeFromTop(componentsMargin);

    auto subArea = area.removeFromTop(buttonHeight);
    subArea.removeFromLeft(buttonMarginX);
    subArea.removeFromRight(buttonMarginX);
    okButton.setBounds(subArea);
}