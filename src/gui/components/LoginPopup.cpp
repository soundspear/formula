#include "LoginPopup.hpp"

formula::gui::LoginPopup::LoginPopup(
	const std::shared_ptr<formula::events::EventHub>& eventHub,
	const std::shared_ptr<formula::processor::PluginState>& pluginState)
	: eventHub(eventHub), pluginState(pluginState),
	subscribeButton("Subscribe now", juce::URL("https://soundspear.com/product/formula"))
{
	auto labelFont = titleLabel.getFont();
	labelFont.setHeight(20);
	titleLabel.setFont(labelFont);
	addAndMakeVisible(titleLabel);

	addAndMakeVisible(descriptionLabel);

	subscribeButton.onClick = [this] {
	};
	addAndMakeVisible(subscribeButton);

	loginLabel.setText("Already have a subscription?\nLog-in to your Soundspear account:", NotificationType::dontSendNotification);
	addAndMakeVisible(loginLabel);

	emailLabel.setText("Email", NotificationType::dontSendNotification);
	addAndMakeVisible(emailLabel);
	addAndMakeVisible(emailEditor);
	passwordLabel.setText("Password", NotificationType::dontSendNotification);
	addAndMakeVisible(passwordLabel);
	passwordEditor.setPasswordCharacter('*');
	addAndMakeVisible(passwordEditor);
	validateLoginButton.setButtonText("Log In");
	addAndMakeVisible(validateLoginButton);

	cancelButton.setButtonText("Cancel");
	cancelButton.onClick = [this] {
		this->setVisible(false);
	};
	addAndMakeVisible(cancelButton);
}

void formula::gui::LoginPopup::setType(LoginPopupType popupType)
{
	switch (popupType) {
	case LoginPopupType::Synchronize:
		titleLabel.setText("Synchronize your formulas", NotificationType::sendNotification);
		descriptionLabel.setText(
			juce::String("A subscription to Formula Cloud allows you to continuously save your ")
			+ juce::String("formulas to your account and synchronize them between your devices."),
			NotificationType::sendNotification
		);
		descriptionLabelHeight = 60;
		break;
	}
}

juce::Rectangle<int> formula::gui::LoginPopup::getAreaToFit(juce::Point<int> parentCenter)
{
	return juce::Rectangle<int>(
		parentCenter.getX() - 200,
		parentCenter.getY() - 175,
		400,
		350
	);
}

void formula::gui::LoginPopup::paint(Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(ListBox::backgroundColourId).brighter(0.1f));
	auto area = getLocalBounds();
	g.setColour(getLookAndFeel().findColour(ListBox::outlineColourId));
	g.drawRect(area);
}

void formula::gui::LoginPopup::resized()
{
	constexpr auto pad = 12;
	constexpr auto topMargin = 10;
	constexpr auto componentsMargin = 14;
	constexpr auto labelMargin = 4;

	constexpr auto titleHeight = 18;
	constexpr auto buttonHeight = 24;
	constexpr auto buttonMarginX = 64;
	constexpr auto loginLabelHeight = 48;
	constexpr auto saveButtonWidth = 225;
	constexpr auto editorLabelWidth = 128;

	auto area = getLocalBounds();
	area = area.withTrimmedTop(pad).withTrimmedBottom(pad).withTrimmedLeft(pad).withTrimmedRight(pad);

	area.removeFromTop(topMargin);

	titleLabel.setBounds(area.removeFromTop(titleHeight));
	area.removeFromTop(componentsMargin);

	descriptionLabel.setBounds(area.removeFromTop(descriptionLabelHeight));
	area.removeFromTop(labelMargin);

	auto subArea = area.removeFromTop(buttonHeight);
	subArea.removeFromLeft(buttonMarginX);
	subArea.removeFromRight(buttonMarginX);
	subscribeButton.setBounds(subArea);
	area.removeFromTop(componentsMargin);

	loginLabel.setBounds(area.removeFromTop(loginLabelHeight));
	area.removeFromTop(labelMargin);

	subArea = area.removeFromTop(buttonHeight);
	auto labelArea = subArea.removeFromLeft(editorLabelWidth);
	emailLabel.setBounds(labelArea);
	emailEditor.setBounds(subArea);

	subArea = area.removeFromTop(buttonHeight);
	labelArea = subArea.removeFromLeft(editorLabelWidth);
	passwordLabel.setBounds(labelArea);
	passwordEditor.setBounds(subArea);

	area.removeFromTop(componentsMargin);
	validateLoginButton.setBounds(area.removeFromTop(buttonHeight));

	subArea = area.removeFromBottom(buttonHeight);
	subArea.removeFromLeft(buttonMarginX);
	subArea.removeFromRight(buttonMarginX);
	cancelButton.setBounds(subArea);

}

