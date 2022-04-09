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
      spinner(eventHubRef)
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

    addAndMakeVisible(spinner);
    spinner.hideSpinner();

    if (!tooltipWindow) {
        tooltipWindow = std::make_unique<TooltipWindow>(nullptr, 200);
    }

    repaint();
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
}
