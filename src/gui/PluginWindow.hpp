#ifndef FORMULA_GUI_PLUGIN_WINDOW_INCLUDED
#define FORMULA_GUI_PLUGIN_WINDOW_INCLUDED

#include <events/EventHub.hpp>
#include <processor/PluginProcessor.hpp>
#include <storage/LocalIndex.hpp>
#include <gui/tabs/CodeEditorTab.hpp>
#include <gui/tabs/SavedFilesTab.hpp>
#include <gui/tabs/OnlineFormulasTab.hpp>
#include <gui/components/SpinnerOverlay.hpp>
#include <gui/FormulaLookAndFeel.hpp>

namespace formula::gui 
{
    class PluginWindow : public juce::AudioProcessorEditor
    {
    public:
        explicit PluginWindow(
            formula::processor::PluginProcessor& processor, 
            const std::shared_ptr<formula::events::EventHub>& eventHub,
            const std::shared_ptr<formula::processor::PluginState>& pluginState,
            const std::shared_ptr<formula::cloud::FormulaCloudClient>& cloud,
            const std::shared_ptr<formula::storage::LocalIndex>& localIndex
        );
        ~PluginWindow() override;

        void paint(juce::Graphics&) override;
        void resized() override;
    private:
        formula::processor::PluginProcessor& processorRef;
        std::shared_ptr<formula::events::EventHub> eventHub;
        std::shared_ptr<formula::processor::PluginState> pluginState;
        std::shared_ptr<formula::cloud::FormulaCloudClient> cloud;

        TabbedComponent tabs;
        SpinnerOverlay spinner;
        static std::unique_ptr<TooltipWindow> tooltipWindow;
        std::unique_ptr<formula::gui::FormulaLookAndFeel> laf;

        float scaleFactor = 1;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginWindow)
    };
}

#endif // FORMULA_GUI_PLUGIN_WINDOW_INCLUDED