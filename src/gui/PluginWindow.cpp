/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#include <gui/PluginWindow.hpp>

std::unique_ptr<TooltipWindow> formula::gui::PluginWindow::tooltipWindow = nullptr;

formula::gui::PluginWindow::PluginWindow(
    formula::processor::PluginProcessor& processorRef,
    const std::shared_ptr<formula::events::EventHub>& eventHubRef,
    const std::shared_ptr<formula::processor::PluginState>& pluginStateRef,
    const std::shared_ptr<formula::storage::CommunityIndex>& communityIndexRef,
    const std::shared_ptr<formula::storage::UserIndex>& userIndexRef,
    const std::shared_ptr<formula::storage::LocalSettings>& settingsRef,
    const std::shared_ptr<formula::processor::FilePlayer>& filePlayerRef
)
    : AudioProcessorEditor (&processorRef),
      associatedProcessor(processorRef),
      eventHub(eventHubRef),
      pluginState(pluginStateRef),
      settings(settingsRef),
      github(eventHubRef),
      tabs(TabbedButtonBar::TabsAtTop),
      spinner(eventHubRef)
{
    setResizable(true, false);
    auto resolutionSetting = settings->find<std::string>(formula::storage::SettingKey::windowResolution);
    setWindowSizeFromResolutionString(resolutionSetting);

    auto colour = findColour(ResizableWindow::backgroundColourId);

    tabs.addTab("Editor", colour, new CodeEditorTab(eventHub, pluginState, userIndexRef), true);
    tabs.addTab("Saved files", colour, new SavedFormulasTab(eventHub, pluginState, userIndexRef), true);
    tabs.addTab("All Formulas", colour, new CommunityFormulasTab(eventHub, communityIndexRef), true);
    tabs.addTab("Settings", colour, new SettingsTab(eventHub, filePlayerRef), true);

    addAndMakeVisible(tabs);

    logoDrawable = Drawable::createFromImageData(formula::binary::logo_svg, formula::binary::logo_svgSize);
    addAndMakeVisible(logoDrawable.get());
    logoDrawable->setAlpha(0.6f);

    versionLabel.setText("v" + juce::String(FORMULA_VERSION), NotificationType::dontSendNotification);
    versionLabel.setAlpha(0.4f);
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
            EventType::newVersionReleased, [] ([[maybe_unused]] boost::any arg, [[maybe_unused]] PluginWindow* thisPtr) {
                auto newVersion = boost::any_cast<std::string>(arg);
                auto result = juce::AlertWindow::showYesNoCancelBox(juce::MessageBoxIconType::InfoIcon,
                  "Update available", juce::String("A new update for Formula is available (version ")
                   + juce::String(newVersion) + "). Do you want to download it?");
                if (result == 1) {
                    const auto url = juce::URL("https://soundspear.com/product/formula");
                    url.launchInDefaultBrowser();
                }
            }, this);
    github.checkForUpdates();

    eventHub->subscribeOnUiThread<PluginWindow>(
            EventType::loadFormulaRequest, [] ([[maybe_unused]] boost::any _, PluginWindow* thisPtr) {
                thisPtr->tabs.setCurrentTabIndex(0);
                thisPtr->resized();
            }, this);

    eventHub->subscribeOnUiThread<PluginWindow>(
            EventType::scaleUp, [] ([[maybe_unused]] boost::any _, PluginWindow* thisPtr) {
                thisPtr->scaleFactor += 0.25f;
                thisPtr->scaleFactor = fmin(2.f, thisPtr->scaleFactor);
                thisPtr->setScaleFactor(thisPtr->scaleFactor);
            }, this);

    eventHub->subscribeOnUiThread<PluginWindow>(
            EventType::scaleDown, [] ([[maybe_unused]] boost::any _, PluginWindow* thisPtr) {
                thisPtr->scaleFactor -= 0.25f;
                thisPtr->scaleFactor = fmax(0.25f, thisPtr->scaleFactor);
                thisPtr->setScaleFactor(thisPtr->scaleFactor);
            }, this);

    eventHub->subscribeOnUiThread<PluginWindow>(
            EventType::unexpectedError, [] ([[maybe_unused]] boost::any arg, [[maybe_unused]] PluginWindow* thisPtr) {
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
}

formula::gui::PluginWindow::~PluginWindow() {
    setLookAndFeel(nullptr);
    if (tooltipWindow) {
        tooltipWindow.reset(nullptr);
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
    noCompilerFoundPopup.setBounds(noCompilerFoundPopup.getAreaToFit(areaCenter));
}

void formula::gui::PluginWindow::setWindowSizeFromResolutionString(std::optional<std::string> resolutionOpt) {
    auto resolutionStr = resolutionOpt.has_value() ? resolutionOpt.value() : "900x450";
    std::vector<std::string> resolution;
    boost::split(resolution, resolutionStr, boost::is_any_of("x"));

    auto resX = boost::lexical_cast<int>(resolution[0]);
    auto resY = boost::lexical_cast<int>(resolution[1]);

    setSize(resX, resY);
}