/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_GUI_SEARCHBAR_INCLUDED
#define FORMULA_GUI_SEARCHBAR_INCLUDED

#include <JuceHeader.h>
#include "gui/components/IconButton.hpp"
#include "events/EventHub.hpp"

namespace formula::gui {
    /**
     * Search bar for Formula name and description
     */
    class SearchBar : public juce::Component {
    public:
        explicit SearchBar(const std::shared_ptr<formula::events::EventHub>& eventHubRef);
        void resized() override;

        std::string getQuery() const { return queryEditor.getText().toStdString(); }
        void setSearchEventType(EventType searchEventType);
    private:
        std::shared_ptr<formula::events::EventHub> eventHub;

        juce::TextEditor queryEditor;
        formula::gui::IconButton searchButton;

        EventType searchEventType;
    };
}


#endif //FORMULA_GUI_SEARCHBAR_INCLUDED
