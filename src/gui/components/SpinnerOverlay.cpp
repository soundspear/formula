#include "SpinnerOverlay.hpp"

using namespace boost::assign;

formula::gui::SpinnerOverlay::SpinnerOverlay(const std::shared_ptr<formula::events::EventHub>& eventHubRef)
    : spinner(progress), eventHub(eventHubRef) {
    std::vector<EventType> spinnerStartEvents;
    spinnerStartEvents += EventType::webRequestSent, EventType::compilationRequest;

    for (const auto& event : spinnerStartEvents) {
        eventHubRef->subscribeOnUiThread<SpinnerOverlay>(
                event, []([[maybe_unused]] boost::any _, SpinnerOverlay* thisPtr) {
            thisPtr->showSpinner();
        }, this);
    }

    std::vector<EventType> spinnerStopEvents;
    spinnerStopEvents += EventType::webRequestFinished, EventType::compilationSuccess, EventType::compilationFail;

    for (const auto& event : spinnerStopEvents) {
        eventHubRef->subscribeOnUiThread<SpinnerOverlay>(
                event, []([[maybe_unused]] boost::any _, SpinnerOverlay* thisPtr) {
            thisPtr->hideSpinner();
        }, this);
    }

    spinner.setPercentageDisplay(false);
    addAndMakeVisible(spinner);
    hideSpinner();
}

formula::gui::SpinnerOverlay::~SpinnerOverlay() {
    eventHub->unsubscribe(this);
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
