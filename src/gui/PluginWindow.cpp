#include <gui/PluginWindow.hpp>

formula::gui::PluginWindow::PluginWindow(
    formula::processor::PluginProcessor& processor,
    const std::shared_ptr<formula::events::EventHub>& eventHub,
    const std::shared_ptr<formula::processor::PluginState>& pluginState,
    const std::shared_ptr<formula::cloud::FormulaCloudClient>& cloud
)
    : AudioProcessorEditor (&processor), 
    processorRef(processor),
    eventHub(eventHub), 
    pluginState(pluginState),
    cloud(cloud),
    tabs(TabbedButtonBar::TabsAtTop),
    spinner(eventHub)
{
    setResizable(true, false);
    setSize (800, 360);
    setScaleFactor(1.5);

    auto colour = findColour(ResizableWindow::backgroundColourId);

    tabs.addTab("Editor", colour, new CodeEditorTab(eventHub, pluginState), true);
    tabs.addTab("Saved files", colour, new SavedFilesTab(eventHub, pluginState), true);
#ifndef FORMULA_LOCAL_ONLY
    tabs.addTab("Browse", colour, new OnlineFormulasTab(eventHub, cloud), true);
#endif
    tabs.addTab("Settings", colour, new Component(), true);

    eventHub->subscribeOnUiThread<PluginWindow>(
            EventType::loadFormulaRequest, [] (boost::any _, PluginWindow* thisPtr) {
        thisPtr->tabs.setCurrentTabIndex(0);
        thisPtr->resized();
    }, this);

    addAndMakeVisible(tabs);

    addAndMakeVisible(spinner);
    spinner.hideSpinner();
    repaint();
}

formula::gui::PluginWindow::~PluginWindow()
{
}

void formula::gui::PluginWindow::paint (juce::Graphics& g)
{
    g.fillAll(Colours::darkred);
}

void formula::gui::PluginWindow::resized()
{
    auto area = getLocalBounds();
    tabs.setBounds(getLocalBounds());
    spinner.setBounds(getLocalBounds());
}
