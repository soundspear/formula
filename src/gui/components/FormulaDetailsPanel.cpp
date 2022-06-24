/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#include "FormulaDetailsPanel.hpp"

#include <memory>

formula::gui::FormulaDetailsPanel::FormulaDetailsPanel(
        const std::shared_ptr<formula::events::EventHub>& eventHubRef
) : eventHub(eventHubRef) {
    nameFont = nameLabel.getFont();
    nameFont.setHeight(20);
    nameLabel.setFont(nameFont);

    closeButton = std::unique_ptr<Button>(getLookAndFeel().createDocumentWindowButton(DocumentWindow::TitleBarButtons::closeButton));
    addAndMakeVisible(closeButton.get());
    closeButton->onClick = [this]() {
        this->setVisible(false);
    };

    addAndMakeVisible(nameLabel);
    authorFont = authorLabel.getFont();
    authorFont.setItalic(true);
    authorLabel.setFont(authorFont);
    addAndMakeVisible(authorLabel);
    descriptionFont = descriptionLabel.getFont();
    descriptionFont.setItalic(true);
    descriptionLabel.setFont(descriptionFont);
    addAndMakeVisible(descriptionLabel);

    codePreviewEditor = std::make_unique<formula::gui::FormulaCodeEditor>(codePreview);
    addAndMakeVisible(codePreviewEditor.get());

    loadFormulaButton.setButtonText("Load formula");
    loadFormulaButton.onClick = [this]() {
        using namespace formula::processor;
        eventHub->publish(EventType::loadFormulaRequest, dto.metadata);
    };
    addAndMakeVisible(loadFormulaButton);

    ratingsHeadingLabel.setText("User ratings", NotificationType::sendNotificationAsync);
#ifdef __PREVIEW_SHOW_RATINGS
    addAndMakeVisible(ratingsHeadingLabel);
#endif
}


void formula::gui::FormulaDetailsPanel::setFormulaDto(formula::cloud::GetFormulaDto newDto) {
    nameLabel.setText(newDto.name, NotificationType::sendNotificationAsync);
    authorLabel.setText("by " + newDto.author, NotificationType::sendNotificationAsync);
    codePreviewEditor->loadContent(newDto.metadata[formula::processor::FormulaMetadataKeys::source]);
    descriptionLabel.setText(newDto.description, NotificationType::sendNotificationAsync);
    this->dto = newDto;

#ifdef __PREVIEW_SHOW_RATINGS
    ratingComponents.clear(); commentLabels.clear();
    for (auto& rating : dto.ratings) {
        auto ratingComponent = std::make_unique<RatingComponent>(true);
        ratingComponent->setRating(rating.rating);
        addAndMakeVisible(ratingComponent.get());
        ratingComponents.push_back(std::move(ratingComponent));

        auto commentLabel = std::make_unique<Label>();
        commentLabel->setText(rating.comment, NotificationType::sendNotificationAsync);
        addAndMakeVisible(commentLabel.get());
        commentLabels.push_back(std::move(commentLabel));
    }
#endif

    resized();
    repaint();
}

void formula::gui::FormulaDetailsPanel::paint(Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ListBox::backgroundColourId).brighter(0.1f));
    auto area = getLocalBounds();
    g.setColour(getLookAndFeel().findColour(ListBox::outlineColourId));
    g.drawRect(area);
}

void formula::gui::FormulaDetailsPanel::resized()
{
    constexpr auto pad = 12;
    constexpr auto borderSize = 1;
    constexpr auto topMargin = 10;
    constexpr auto componentsMargin = 14;
    constexpr auto labelMargin = 4;

    constexpr auto closeButtonSize = 32;
    constexpr auto nameHeight = 18;
    constexpr auto descriptionHeight = 128;
    constexpr auto editorPreviewHeight = 150;
    constexpr auto loadButtonHeight = 32;

    auto area = getLocalBounds();

    closeButton->setBounds(area.getX() + area.getWidth() - closeButtonSize - borderSize, borderSize,
                           closeButtonSize, closeButtonSize);

    area = area.withTrimmedTop(pad).withTrimmedBottom(pad).withTrimmedLeft(pad).withTrimmedRight(pad);

    area.removeFromTop(topMargin);

    nameLabel.setBounds(area.removeFromTop(nameHeight));
    area.removeFromTop(labelMargin);
    authorLabel.setBounds(area.removeFromTop(nameHeight));
    area.removeFromTop(componentsMargin);

    codePreviewEditor->setBounds(area.removeFromTop(editorPreviewHeight));
    area.removeFromTop(componentsMargin);

    descriptionLabel.setBounds(area.removeFromTop(descriptionHeight));
    area.removeFromTop(componentsMargin);

    auto buttonArea = getLocalBounds()
            .withTrimmedBottom(componentsMargin);
    auto buttonMarginX =  buttonArea.getWidth() * 0.4;
    buttonArea = buttonArea.removeFromBottom(loadButtonHeight)
            .withTrimmedLeft(buttonMarginX/2).withTrimmedRight(buttonMarginX/2);
    loadFormulaButton.setBounds(buttonArea);

#ifdef __PREVIEW_SHOW_RATINGS
    constexpr auto commentHeight = 32;
    constexpr auto labelHeight = 12;
    constexpr auto ratingHeight = 25;
    constexpr auto ratingWidth = 80;

    ratingsHeadingLabel.setBounds(area.removeFromTop(labelHeight));
    area.removeFromTop(labelMargin);

    const auto numRatings = ratingComponents.size();
    for (auto i = 0; i < numRatings; i++) {
        auto ratingArea = area.removeFromTop(ratingHeight);
        ratingArea.setWidth(ratingWidth);
        ratingComponents[i]->setBounds(ratingArea);
        area.removeFromTop(labelMargin);
        commentLabels[i]->setBounds(area.removeFromTop(commentHeight));
        area.removeFromTop(labelMargin);
    }
#endif
}

