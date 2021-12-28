#include "SpinnerOverlay.hpp"

using namespace boost::assign;

formula::gui::SpinnerOverlay::SpinnerOverlay(std::shared_ptr<formula::events::EventHub> eventHub)
    : spinner(progress) {
    std::vector<EventType> spinnerStartEvents;
    spinnerStartEvents += EventType::webRequestSent, EventType::compilationRequest;

    for (const auto& event : spinnerStartEvents) {
        eventHub->subscribeOnUiThread<SpinnerOverlay>(
                event, [](boost::any _, SpinnerOverlay* thisPtr) {
            thisPtr->showSpinner();
        }, this);
    }

    std::vector<EventType> spinnerStopEvents;
    spinnerStopEvents += EventType::webRequestFinished, EventType::compilationSuccess, EventType::compilationFail;

    for (const auto& event : spinnerStopEvents) {
        eventHub->subscribeOnUiThread<SpinnerOverlay>(
                event, [](boost::any _, SpinnerOverlay* thisPtr) {
            thisPtr->hideSpinner();
        }, this);
    }

    spinner.setPercentageDisplay(false);
    addAndMakeVisible(spinner);
    hideSpinner();
}

void formula::gui::SpinnerOverlay::showSpinner() {
    setVisible(true);
}

void formula::gui::SpinnerOverlay::hideSpinner() {
    setVisible(false);
}

void formula::gui::SpinnerOverlay::resized() {
    spinner.centreWithSize(100, 100);
}

void formula::gui::SpinnerOverlay::paint(juce::Graphics& g) {
    //g.fillAll(juce::Colour(0x22000000));
}
