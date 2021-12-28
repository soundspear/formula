#ifndef FORMULA_GUI_SPINNEROVERLAY_INCLUDED
#define FORMULA_GUI_SPINNEROVERLAY_INCLUDED

#include <boost/assign/std/vector.hpp>
#include <boost/any.hpp>

#include <JuceHeader.h>

#include <events/EventHub.hpp>

namespace formula::gui {
    class SpinnerOverlay : public Component {
    public:
        explicit SpinnerOverlay(std::shared_ptr<formula::events::EventHub> eventHub);
        void showSpinner();
        void hideSpinner();

        void paint(juce::Graphics&) override;
        void resized() override;
    private:
        juce::ProgressBar spinner;
        double progress = -1.0;
    };
}

#endif //FORMULA_GUI_SPINNEROVERLAY_INCLUDED
