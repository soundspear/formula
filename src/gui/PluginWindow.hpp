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
            const std::shared_ptr<formula::events::EventHub>& eventHubRef,
            const std::shared_ptr<formula::processor::PluginState>& pluginStateRef,
            const std::shared_ptr<formula::cloud::FormulaCloudClient>& cloudRef,
            const std::shared_ptr<formula::storage::LocalIndex>& localIndexRef
        );
        ~PluginWindow() override;

        void paint(juce::Graphics&) override;
        void resized() override;
    private:
        formula::processor::PluginProcessor& associatedProcessor;
        std::shared_ptr<formula::events::EventHub> eventHub;
        std::shared_ptr<formula::processor::PluginState> pluginState;
        std::shared_ptr<formula::cloud::FormulaCloudClient> cloud;

        std::unique_ptr<juce::Drawable> logoDrawable;
        juce::Rectangle<float> logoPos;
        juce::Label versionLabel;
        juce::Font versionFont;

        juce::TabbedComponent tabs;
        SpinnerOverlay spinner;
        static std::unique_ptr<juce::TooltipWindow> tooltipWindow;
        std::unique_ptr<formula::gui::FormulaLookAndFeel> laf;

        float scaleFactor = 1;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginWindow)
    };
}

#endif // FORMULA_GUI_PLUGIN_WINDOW_INCLUDED