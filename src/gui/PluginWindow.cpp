#include <gui/PluginWindow.hpp>

std::unique_ptr<TooltipWindow> formula::gui::PluginWindow::tooltipWindow = nullptr;

formula::gui::PluginWindow::PluginWindow(
    formula::processor::PluginProcessor& processor,
    const std::shared_ptr<formula::events::EventHub>& eventHubRef,
    const std::shared_ptr<formula::processor::PluginState>& pluginStateRef,
    const std::shared_ptr<formula::cloud::FormulaCloudClient>& cloudRef,
    const std::shared_ptr<formula::storage::LocalIndex>& localIndexRef
)
    : AudioProcessorEditor (&processor),
      associatedProcessor(processor),
      eventHub(eventHubRef),
      pluginState(pluginStateRef),
      cloud(cloudRef),
      tabs(TabbedButtonBar::TabsAtTop),
      spinner(eventHubRef),
      loginPopup(cloud)
{
    if (!laf) {
        laf = std::make_unique<FormulaLookAndFeel>();
    }
    setLookAndFeel(laf.get());

    setResizable(true, false);
    setSize(900, 450);
    auto colour = findColour(ResizableWindow::backgroundColourId);

    tabs.addTab("Editor", colour, new CodeEditorTab(eventHub, pluginState, localIndexRef), true);
    tabs.addTab("Saved files", colour, new SavedFilesTab(eventHub, pluginState, localIndexRef), true);
#ifndef FORMULA_LOCAL_ONLY
    tabs.addTab("Browse", colour, new OnlineFormulasTab(eventHub, cloud), true);
#endif
    tabs.addTab("Settings", colour, new Component(), true);

    addAndMakeVisible(tabs);

    logoDrawable = Drawable::createFromImageData(formula::binary::logo_svg, formula::binary::logo_svgSize);
    addAndMakeVisible(logoDrawable.get());

    versionLabel.setText("v" + juce::String(FORMULA_VERSION), NotificationType::dontSendNotification);
    versionLabel.setAlpha(0.6);
    versionFont = versionLabel.getFont();
    versionFont.setHeight(12);
    versionLabel.setFont(versionFont);
    addAndMakeVisible(versionLabel);

    addChildComponent(loginPopup);

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

    eventHub->subscribeOnUiThread<PluginWindow>(
            EventType::loadFormulaRequest, [] ([[maybe_unused]] boost::any _, PluginWindow* thisPtr) {
        thisPtr->tabs.setCurrentTabIndex(0);
        thisPtr->resized();
    }, this);

    eventHub->subscribeOnUiThread<PluginWindow>(
            EventType::scaleUp, [] ([[maybe_unused]] boost::any _, PluginWindow* thisPtr) {
        thisPtr->scaleFactor += 0.25;
        thisPtr->setScaleFactor(thisPtr->scaleFactor);
    }, this);

    eventHub->subscribeOnUiThread<PluginWindow>(
            EventType::scaleDown, [] ([[maybe_unused]] boost::any _, PluginWindow* thisPtr) {
        thisPtr->scaleFactor -= 0.25;
        thisPtr->setScaleFactor(thisPtr->scaleFactor);
    }, this);

    eventHub->subscribeOnUiThread<PluginWindow>(
            EventType::unknownWebError, [] ([[maybe_unused]] boost::any _, PluginWindow* thisPtr) {
        juce::AlertWindow::showMessageBox(juce::MessageBoxIconType::WarningIcon,
                                          "Network error", "An unknown network error happened");
    }, this);

    addAndMakeVisible(spinner);
    spinner.hideSpinner();

    if (!tooltipWindow) {
        tooltipWindow = std::make_unique<TooltipWindow>(nullptr, 200);
    }

    resized();
}

formula::gui::PluginWindow::~PluginWindow() {
    setLookAndFeel(nullptr);
    if (tooltipWindow) {
        tooltipWindow.reset(nullptr);
    }
    if (laf) {
        laf.reset(nullptr);
    }
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
}
