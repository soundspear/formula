/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#include <gui/components/RatingComponent.hpp>

formula::gui::RatingComponent::RatingComponent(bool readonly) : isReadOnly(readonly) {
    for (auto i = 0; i < 5; i++) {
        stars.push_back(designButton());
        stars.back()->setEnabled(false);
        addAndMakeVisible(*stars.back());
    }
}

void formula::gui::RatingComponent::paint(Graphics&) {
    const auto buttonSize = getWidth() / 5;
    for (auto i = 0u; i < stars.size(); i++) {
        stars[i]->setBounds(static_cast<int>(i) * buttonSize, 0, buttonSize, buttonSize);
    }
}

void formula::gui::RatingComponent::setReadOnly(bool readonly) {
    this->isReadOnly = readonly;
}

std::unique_ptr<juce::DrawableButton> formula::gui::RatingComponent::designButton() {
    std::unique_ptr<juce::DrawableButton> button(std::make_unique<juce::DrawableButton>("", DrawableButton::ImageStretched) );
    DrawablePath normal, over;

    Path p;
    p.addStar ({}, 5, 20.0f, 50.0f, 0);
    normal.setPath (p);
    normal.setFill (Colours::lightblue);

    over.setPath (p);
    over.setFill (Colours::blue);
    button->setImages (&normal, &over,  &over);

    return button;
}

void formula::gui::RatingComponent::setRating(double rating) {
    for (auto i = 0u; i < 5; i++) {
        auto visible = i <= rating;
        stars[i]->setVisible(visible);
    }
}

