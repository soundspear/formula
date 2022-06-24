/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_GUI_SPINNEROVERLAY_INCLUDED
#define FORMULA_GUI_SPINNEROVERLAY_INCLUDED

#include <boost/assign/std/vector.hpp>
#include <boost/any.hpp>

#include <JuceHeader.h>

#include <events/EventHub.hpp>

namespace formula::gui {
    class SpinnerOverlay : public Component {
    public:
        explicit SpinnerOverlay(const std::shared_ptr<formula::events::EventHub>& eventHub);
        ~SpinnerOverlay();
        void showSpinner();
        void hideSpinner();

        void paint(juce::Graphics&) override;
        void resized() override;
    private:
        std::shared_ptr<formula::events::EventHub> eventHub;

        juce::ProgressBar spinner;
        double progress = -1.0;
    };
}

#endif //FORMULA_GUI_SPINNEROVERLAY_INCLUDED
