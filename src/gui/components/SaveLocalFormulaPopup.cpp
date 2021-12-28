#include "SaveLocalFormulaPopup.hpp"

formula::gui::SaveLocalFormulaPopup::SaveLocalFormulaPopup(
	const std::shared_ptr<formula::events::EventHub>& eventHub, 
	const std::shared_ptr<formula::processor::PluginState>& pluginState)
	: eventHub(eventHub), pluginState(pluginState)
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
		const auto name = nameEditor.getText().trim().toStdString();
		if (name.empty()) {
			return;
		}

		auto metadata = this->pluginState->getActiveFormulaMetadata();
		metadata[formula::processor::FormulaMetadataKeys::name] = name;
		metadata[formula::processor::FormulaMetadataKeys::description] = descriptionEditor.getText().trim().toStdString();

		this->pluginState->setActiveFormulaMetadata(metadata);

		this->eventHub->publish(EventType::saveLocalFormulaRequest);
		this->setVisible(false);
	};
	addAndMakeVisible(saveButton);
	cancelButton.setButtonText("Cancel");
	cancelButton.onClick = [this] {
		this->setVisible(false);
	};
	addAndMakeVisible(cancelButton);
}

void formula::gui::SaveLocalFormulaPopup::paint(Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(ListBox::backgroundColourId).brighter(0.1f));
	auto area = getLocalBounds();
	g.setColour(getLookAndFeel().findColour(ListBox::outlineColourId));
	g.drawRect(area);
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
