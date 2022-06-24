/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#include "SaveLocalFormulaPopup.hpp"

formula::gui::SaveLocalFormulaPopup::SaveLocalFormulaPopup(
	const std::shared_ptr<formula::storage::LocalIndex>& localIndexRef,
	const std::shared_ptr<formula::processor::PluginState>& pluginStateRef)
	: localIndex(localIndexRef), pluginState(pluginStateRef)
{
	titleLabel.setText("Save Formula to local storage", NotificationType::dontSendNotification);
	auto labelFont = titleLabel.getFont();
	labelFont.setHeight(20);
	titleLabel.setFont(labelFont);
	addAndMakeVisible(titleLabel);
	nameLabel.setText("Formula name", NotificationType::dontSendNotification);
	addAndMakeVisible(nameLabel);
	descriptionLabel.setText("Formula description", NotificationType::dontSendNotification);
	addAndMakeVisible(descriptionLabel);

	addAndMakeVisible(nameEditor);
	descriptionEditor.setMultiLine(true);
	addAndMakeVisible(descriptionEditor);

	saveButton.setButtonText("Save");
	saveButton.onClick = [this] {
        SafePointer<SaveLocalFormulaPopup> thisPtr(this);
        MessageManager::callAsync([thisPtr]() {
            const auto name = thisPtr->nameEditor.getText().trim().toStdString();
            if (name.empty()) {
                return;
            }

            auto metadata = thisPtr->pluginState->getActiveFormulaMetadata();
            metadata[formula::processor::FormulaMetadataKeys::name] = name;
            metadata[formula::processor::FormulaMetadataKeys::description] = thisPtr->descriptionEditor.getText().trim().toStdString();

            thisPtr->pluginState->setActiveFormulaMetadata(metadata);

            thisPtr->setVisible(false);
            thisPtr->localIndex->saveCurrentFormulaToIndex();
        });
	};
	addAndMakeVisible(saveButton);
	cancelButton.setButtonText("Cancel");
	cancelButton.onClick = [this] {
		this->setVisible(false);
	};
	addAndMakeVisible(cancelButton);
}

juce::Point<int> formula::gui::SaveLocalFormulaPopup::getPopupSize() {
    return { 400, 350 };
}

void formula::gui::SaveLocalFormulaPopup::resized()
{
	constexpr auto pad = 12;
	constexpr auto topMargin = 10;
	constexpr auto componentsMargin = 14;
	constexpr auto labelMargin = 4;

	constexpr auto titleHeight = 18;
	constexpr auto labelHeight = 28;
	constexpr auto nameEditorHeight = 24;
	constexpr auto descriptionEditorHeight = 140;
	constexpr auto buttonHeight = 24;
	constexpr auto saveButtonWidth = 225;

	auto area = getLocalBounds();
	area = area.withTrimmedTop(pad).withTrimmedBottom(pad).withTrimmedLeft(pad).withTrimmedRight(pad);

	area.removeFromTop(topMargin);

	titleLabel.setBounds(area.removeFromTop(titleHeight));
	area.removeFromTop(componentsMargin);

	nameLabel.setBounds(area.removeFromTop(labelHeight));
	area.removeFromTop(labelMargin);
	nameEditor.setBounds(area.removeFromTop(nameEditorHeight));
	area.removeFromTop(componentsMargin);

	descriptionLabel.setBounds(area.removeFromTop(labelHeight));
	area.removeFromTop(labelMargin);
	descriptionEditor.setBounds(area.removeFromTop(descriptionEditorHeight));
	area.removeFromTop(componentsMargin);

	auto buttonsArea = area.removeFromTop(buttonHeight);
	saveButton.setBounds(buttonsArea.removeFromLeft(saveButtonWidth));
	buttonsArea.removeFromLeft(componentsMargin);
	cancelButton.setBounds(buttonsArea);
}

void formula::gui::SaveLocalFormulaPopup::resetContent()
{
	nameEditor.setText("");
	descriptionEditor.setText("");
}

void formula::gui::SaveLocalFormulaPopup::visibilityChanged()
{
	auto metadata = pluginState->getActiveFormulaMetadata();
	nameEditor.setText(metadata[formula::processor::FormulaMetadataKeys::name]);
	descriptionEditor.setText(metadata[formula::processor::FormulaMetadataKeys::description]);
}