/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#include <gui/PluginWindow.hpp>

std::unique_ptr<TooltipWindow> formula::gui::PluginWindow::tooltipWindow = nullptr;

formula::gui::PluginWindow::PluginWindow(
    formula::processor::PluginProcessor& processor,
    const std::shared_ptr<formula::events::EventHub>& eventHubRef,
    const std::shared_ptr<formula::processor::PluginState>& pluginStateRef,
    const std::shared_ptr<formula::cloud::FormulaCloudClient>& cloudRef,
    const std::shared_ptr<formula::storage::LocalIndex>& localIndexRef,
    const std::shared_ptr<formula::storage::LocalSettings>& settingsRef
)
    : AudioProcessorEditor (&processor),
      associatedProcessor(processor),
      eventHub(eventHubRef),
      pluginState(pluginStateRef),
      cloud(cloudRef),
      settings(settingsRef),
      tabs(TabbedButtonBar::TabsAtTop),
      spinner(eventHubRef),
      loginPopup(cloud),
      setUserNamePopup(cloud)
{
    if (!laf) {
        laf = std::make_unique<FormulaLookAndFeel>();
    }
    LookAndFeel::setDefaultLookAndFeel(laf.get());

    setResizable(true, false);
    auto resolutionSetting = settings->find<std::string>(formula::storage::SettingKey::windowResolution);
    setWindowSizeFromResolutionString(resolutionSetting);

    auto colour = findColour(ResizableWindow::backgroundColourId);

    tabs.addTab("Editor", colour, new CodeEditorTab(eventHub, pluginState, localIndexRef), true);
    tabs.addTab("Saved files", colour, new SavedFilesTab(eventHub, cloud, pluginState, localIndexRef), true);
#ifndef FORMULA_LOCAL_ONLY
    tabs.addTab("Formula cloud", colour, new OnlineFormulasTab(eventHub, cloud), true);
#endif
    tabs.addTab("Settings", colour, new SettingsTab(eventHub, cloud), true);

    addAndMakeVisible(tabs);

    logoDrawable = Drawable::createFromImageData(formula::binary::logo_svg, formula::binary::logo_svgSize);
    addAndMakeVisible(logoDrawable.get());
    logoDrawable->setAlpha(0.6);

    versionLabel.setText("v" + juce::String(FORMULA_VERSION), NotificationType::dontSendNotification);
    versionLabel.setAlpha(0.4);
    versionFont = versionLabel.getFont();
    versionFont.setHeight(12);
    versionLabel.setFont(versionFont);
    addAndMakeVisible(versionLabel);

    addAndMakeVisible(spinner);
    spinner.hideSpinner();

    if (!tooltipWindow) {
        tooltipWindow = std::make_unique<TooltipWindow>(nullptr, 200);
    }

    setupPopups();

    eventHub->subscribeOnUiThread<PluginWindow>(
            EventType::loadFormulaRequest, [] ([[maybe_unused]] boost::any _, PluginWindow* thisPtr) {
                thisPtr->tabs.setCurrentTabIndex(0);
                thisPtr->resized();
            }, this);

    eventHub->subscribeOnUiThread<PluginWindow>(
            EventType::scaleUp, [] ([[maybe_unused]] boost::any _, PluginWindow* thisPtr) {
                thisPtr->scaleFactor += 0.25;
                thisPtr->scaleFactor = fmin(2.f, thisPtr->scaleFactor);
                thisPtr->setScaleFactor(thisPtr->scaleFactor);
            }, this);

    eventHub->subscribeOnUiThread<PluginWindow>(
            EventType::scaleDown, [] ([[maybe_unused]] boost::any _, PluginWindow* thisPtr) {
                thisPtr->scaleFactor -= 0.25;
                thisPtr->scaleFactor = fmax(0.25f, thisPtr->scaleFactor);
                thisPtr->setScaleFactor(thisPtr->scaleFactor);
            }, this);

    eventHub->subscribeOnUiThread<PluginWindow>(
            EventType::unexpectedError, [] ([[maybe_unused]] boost::any arg, PluginWindow* thisPtr) {
                formula::gui::ErrorCodes errCode = formula::gui::ErrorCodes::unknownError;
                if (!arg.empty()) {
                    errCode = boost::any_cast<formula::gui::ErrorCodes>(arg);
                }
                juce::AlertWindow::showMessageBox(juce::MessageBoxIconType::WarningIcon,
                                                  "Network error", juce::String("An unexpected error happened.")
                                                                   + " (Error code " + juce::String(errCode) + ")");
            }, this);

    eventHub->subscribeOnUiThread<PluginWindow>(
            EventType::windowSizeRequested, [] ([[maybe_unused]] boost::any arg, PluginWindow* thisPtr) {
                auto resolutionStr = boost::any_cast<std::string>(arg);
                thisPtr->setWindowSizeFromResolutionString(resolutionStr);
                thisPtr->settings->add<std::string>(formula::storage::SettingKey::windowResolution, resolutionStr);
            }, this);

    resized();
}

void formula::gui::PluginWindow::setupPopups() {

    // No compiler found popup
    addChildComponent(noCompilerFoundPopup);

    eventHub->subscribeOnUiThread<PluginWindow>(
            EventType::noCompilerFound, [] ([[maybe_unused]] boost::any _, PluginWindow* thisPtr) {
                thisPtr->noCompilerFoundPopup.setVisible(true);
                thisPtr->tabs.setInterceptsMouseClicks(false, false);
                thisPtr->resized();
            }, this);

    // Login Popup
    addChildComponent(loginPopup);

    eventHub->subscribeOnUiThread<PluginWindow>(
            EventType::loginSuccess, [] ([[maybe_unused]] boost::any _, PluginWindow* thisPtr) {
                juce::AlertWindow::showMessageBox(juce::AlertWindow::InfoIcon,
                                                  "Login success!", "You are logged into Formula Cloud");
            }, this);

    eventHub->subscribeOnUiThread<PluginWindow>(
            EventType::loginFail, [] ([[maybe_unused]] boost::any _, PluginWindow* thisPtr) {
                thisPtr->loginPopup.setType(LoginPopupType::LoginFailed);
                thisPtr->loginPopup.setVisible(true); thisPtr->resized();
            }, this);

    eventHub->subscribeOnUiThread<PluginWindow>(
            EventType::needLogin, [] ([[maybe_unused]] boost::any _, PluginWindow* thisPtr) {
                thisPtr->loginPopup.setType(LoginPopupType::FirstLogin);
                thisPtr->loginPopup.setVisible(true); thisPtr->resized();
            }, this);

    eventHub->subscribeOnUiThread<PluginWindow>(
            EventType::subscriptionExpired, [] ([[maybe_unused]] boost::any _, PluginWindow* thisPtr) {
                thisPtr->loginPopup.setType(LoginPopupType::MissingSubscription);
                thisPtr->loginPopup.setVisible(true); thisPtr->resized();
            }, this);

    // Set UserName Popup
    addChildComponent(setUserNamePopup);

    eventHub->subscribeOnUiThread<PluginWindow>(
            EventType::needSetUsername, [] ([[maybe_unused]] boost::any _, PluginWindow* thisPtr) {
                thisPtr->setUserNamePopup.setUserNameAlreadyExists(false);
                thisPtr->setUserNamePopup.setVisible(true); thisPtr->resized();
            }, this);

    eventHub->subscribeOnUiThread<PluginWindow>(
            EventType::userNameAlreadyExists, [] ([[maybe_unused]] boost::any _, PluginWindow* thisPtr) {
                thisPtr->setUserNamePopup.setUserNameAlreadyExists(true);
                thisPtr->setUserNamePopup.setVisible(true); thisPtr->resized();
            }, this);
}

formula::gui::PluginWindow::~PluginWindow() {
    setLookAndFeel(nullptr);
    if (tooltipWindow) {
        tooltipWindow.reset(nullptr);
    }
    if (laf) {
        laf.reset(nullptr);
    }
    eventHub->unsubscribe(this);
}

void formula::gui::PluginWindow::paint (juce::Graphics& g)
{
    const auto idleColour = Colour::fromRGB(0x3c, 0x3f, 0x41);
    g.fillAll(idleColour);
}

void formula::gui::PluginWindow::resized()
{
    auto area = getLocalBounds();
    tabs.setBounds(area);
    spinner.setBounds(area);

    logoPos = area.withTrimmedRight(45).removeFromRight(55).removeFromTop(31)
            .withTrimmedBottom(15).withTrimmedTop(15).toFloat();
    if (logoDrawable) {
        logoDrawable->setTransformToFit(logoPos, RectanglePlacement::centred | RectanglePlacement::fillDestination);
    }
    versionLabel.setBounds(getLocalBounds().removeFromRight(45).removeFromTop(31)
                                   .withTrimmedBottom(11).withTrimmedTop(10));

    const auto areaCenter = area.getCentre();
    loginPopup.setBounds(loginPopup.getAreaToFit(areaCenter));
    noCompilerFoundPopup.setBounds(noCompilerFoundPopup.getAreaToFit(areaCenter));
    setUserNamePopup.setBounds(setUserNamePopup.getAreaToFit(areaCenter));
}

void formula::gui::PluginWindow::setWindowSizeFromResolutionString(std::optional<std::string> resolutionOpt) {
    auto resolutionStr = resolutionOpt.has_value() ? resolutionOpt.value() : "900x450";
    std::vector<std::string> resolution;
    boost::split(resolution, resolutionStr, boost::is_any_of("x"));

    auto resX = boost::lexical_cast<int>(resolution[0]);
    auto resY = boost::lexical_cast<int>(resolution[1]);

    setSize(resX, resY);
}