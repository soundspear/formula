/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#include "SpinnerOverlay.hpp"

using namespace boost::assign;

formula::gui::SpinnerOverlay::SpinnerOverlay(const std::shared_ptr<formula::events::EventHub>& eventHubRef)
    : eventHub(eventHubRef), spinner(progress){
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

void formula::gui::SpinnerOverlay::paint(juce::Graphics&) { }
