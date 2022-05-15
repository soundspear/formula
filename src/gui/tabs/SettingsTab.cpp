#include "SettingsTab.hpp"

formula::gui::SettingsTab::SettingsTab(
        const std::shared_ptr<formula::events::EventHub>& eventHubRef,
        const std::shared_ptr<formula::cloud::FormulaCloudClient>& cloudRef
)
: eventHub(eventHubRef), cloud(cloudRef)
{
    refreshLoginSettings();
    addAndMakeVisible(loginLabel);
    addAndMakeVisible(loginButton);

    formulaCloudTosButton.setButtonText("Formula Cloud Terms of Service");
    addAndMakeVisible(formulaCloudTosButton);
    formulaCloudTosButton.onClick = [this]() {
        tosPopup.setVisible(true);
    };

    addChildComponent(tosPopup);
}

void formula::gui::SettingsTab::refreshLoginSettings() {
    if (!isVisible()) {
        return;
    }

    if (cloud->isLoggedIn()) {
        std::string labelText = "Logged in";
        auto username = cloud->getUserName();
        if (username) {
            labelText += " as " + username.value();
        }
        loginLabel.setText(labelText, NotificationType::sendNotification);
        loginButton.setButtonText("Log out");
        loginButton.onClick = [this] () {
            if (!cloud->isLoggedIn()) {
                refreshLoginSettings();
                return;
            }
            cloud->logout();
            refreshLoginSettings();
        };
    }
    else {
        loginLabel.setText("You are not logged in", NotificationType::sendNotification);
        loginButton.setButtonText("Log in");
        loginButton.onClick = [this] () {
            if (cloud->isLoggedIn()) {
                refreshLoginSettings();
                return;
            }
            eventHub->publish(EventType::needLogin);
        };
    }
}

void formula::gui::SettingsTab::visibilityChanged() {
    refreshLoginSettings();
    repaint();
}

void formula::gui::SettingsTab::resized() {
    constexpr auto pad = 18;
    constexpr auto componentMargin = 8;

    auto area = getLocalBounds();
    auto colWidth = area.getWidth() / 3;

    const auto areaCenter = area.getCentre();
    tosPopup.setBounds(tosPopup.getAreaToFit(areaCenter));

    auto col1 = area.removeFromLeft(colWidth);
    col1 = col1.withTrimmedBottom(pad).withTrimmedTop(pad).withTrimmedRight(pad).withTrimmedLeft(pad);
    loginLabel.setBounds(col1.removeFromTop(15));
    col1.removeFromTop(componentMargin);
    loginButton.setBounds(col1.removeFromTop(24));
    col1.removeFromTop(componentMargin);
    formulaCloudTosButton.setBounds(col1.removeFromTop(24));
}