#include "LoginPopup.hpp"

formula::gui::LoginPopup::LoginPopup(
	const std::shared_ptr<formula::cloud::FormulaCloudClient>& cloudRef)
	: cloud(cloudRef)
{
	auto labelFont = titleLabel.getFont();
	labelFont.setHeight(20);
	titleLabel.setFont(labelFont);
	addAndMakeVisible(titleLabel);

	addAndMakeVisible(descriptionLabel);

	addAndMakeVisible(linkButton);

	emailLabel.setText("Email", NotificationType::dontSendNotification);
	addAndMakeVisible(emailLabel);
	addAndMakeVisible(emailEditor);
	passwordLabel.setText("Password", NotificationType::dontSendNotification);
	addAndMakeVisible(passwordLabel);
	passwordEditor.setPasswordCharacter('*');
	addAndMakeVisible(passwordEditor);

    validateButton.onClick = [this] {
        this->setVisible(false);
        if (this->popupType == LoginPopupType::MissingSubscription) {
            return;
        }
        cloud->login(emailEditor.getText().toStdString(), passwordEditor.getText().toStdString());
    };
	addAndMakeVisible(validateButton);

    closeButton = std::unique_ptr<Button>(getLookAndFeel().createDocumentWindowButton(DocumentWindow::TitleBarButtons::closeButton));
    addAndMakeVisible(closeButton.get());
    closeButton->onClick = [this]() {
        this->setVisible(false);
    };
	addAndMakeVisible(closeButton.get());
}

void formula::gui::LoginPopup::setType(LoginPopupType newPopupType)
{
    popupType = newPopupType;
	switch (popupType) {
	case LoginPopupType::FirstLogin:
		titleLabel.setText("Formula Cloud login", NotificationType::sendNotification);
		descriptionLabel.setText(
			juce::String("Please log in to your Soundspear account,\nor create one by clicking the link below."),
			NotificationType::sendNotification
		);
        linkButton.setButtonText("Click here to create an account");
        linkButton.setURL(juce::URL("https://soundspear.com/register"));
        linkButton.setVisible(true);
        emailLabel.setVisible(true); passwordLabel.setVisible(true);
        emailEditor.setVisible(true); passwordEditor.setVisible(true);
        validateButton.setButtonText("Log In");
		descriptionLabelHeight = 60;
		break;
    case LoginPopupType::LoginFailed:
        titleLabel.setText("Login failed", NotificationType::sendNotification);
        descriptionLabel.setText(
                juce::String("Invalid username or email, please try again!"),
                NotificationType::sendNotification
        );
        linkButton.setButtonText("Forgot your password?");
        linkButton.setURL(juce::URL("https://soundspear.com/forgotpassword"));
        linkButton.setVisible(true);
        emailLabel.setVisible(true); passwordLabel.setVisible(true);
        emailEditor.setVisible(true); passwordEditor.setVisible(true);
        validateButton.setButtonText("Log In");
        descriptionLabelHeight = 30;
        break;
    case LoginPopupType::MissingSubscription:
        titleLabel.setText("Subscription not found or expired", NotificationType::sendNotification);
        descriptionLabel.setText(
                juce::String("You need a subscription in order to use Formula Cloud."),
                NotificationType::sendNotification
        );
        linkButton.setButtonText("Subscribe now");
        linkButton.setURL(juce::URL("https://soundspear.com/product/formula"));
        linkButton.setVisible(true);
        emailLabel.setVisible(false); passwordLabel.setVisible(false);
        emailEditor.setVisible(false); passwordEditor.setVisible(false);
        validateButton.setButtonText("Ok");
        descriptionLabelHeight = 60;
        break;
    default:
        break;
	}
    resized();
}

juce::Point<int> formula::gui::LoginPopup::getPopupSize() {
    int height = emailLabel.isVisible() ? 275 : 145;
    return { 400, height };
}

void formula::gui::LoginPopup::resized()
{
    constexpr auto closeButtonSize = 32;

	constexpr auto pad = 12;
	constexpr auto topMargin = 8;
	constexpr auto componentsMargin = 12;
	constexpr auto labelMargin = 4;

	constexpr auto titleHeight = 18;
	constexpr auto buttonHeight = 24;
	constexpr auto buttonMarginX = 64;
	constexpr auto editorLabelWidth = 128;

    auto area = getLocalBounds();

    closeButton->setBounds(area.getX() + area.getWidth() - closeButtonSize - 1, 1,
                           closeButtonSize, closeButtonSize);

	area = area.withTrimmedTop(pad).withTrimmedBottom(pad).withTrimmedLeft(pad).withTrimmedRight(pad);

	area.removeFromTop(topMargin);

	titleLabel.setBounds(area.removeFromTop(titleHeight));
	area.removeFromTop(componentsMargin);

	descriptionLabel.setBounds(area.removeFromTop(descriptionLabelHeight));
	area.removeFromTop(labelMargin);

	auto subArea = area.removeFromTop(buttonHeight);
	subArea.removeFromLeft(buttonMarginX);
	subArea.removeFromRight(buttonMarginX);
	linkButton.setBounds(subArea);
	area.removeFromTop(componentsMargin);

    if (emailLabel.isVisible()) {
        subArea = area.removeFromTop(buttonHeight);
        auto labelArea = subArea.removeFromLeft(editorLabelWidth);
        emailLabel.setBounds(labelArea);
        emailEditor.setBounds(subArea);
        area.removeFromTop(componentsMargin);
    }

    if (passwordLabel.isVisible()) {
        subArea = area.removeFromTop(buttonHeight);
        auto labelArea = subArea.removeFromLeft(editorLabelWidth);
        passwordLabel.setBounds(labelArea);
        passwordEditor.setBounds(subArea);
        area.removeFromTop(componentsMargin);
    }

    subArea = area.removeFromTop(buttonHeight);
    subArea.removeFromLeft(buttonMarginX);
    subArea.removeFromRight(buttonMarginX);
    validateButton.setBounds(subArea);
}

