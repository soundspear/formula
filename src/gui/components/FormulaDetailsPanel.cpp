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
    descriptionFont = descriptionLabel.getFont();
    descriptionFont.setItalic(true);
    descriptionLabel.setFont(descriptionFont);
    addAndMakeVisible(descriptionLabel);

    codePreviewEditor = std::make_unique<formula::gui::FormulaCodeEditor>(codePreview);
    addAndMakeVisible(codePreviewEditor.get());

    loadFormulaButton.setButtonText("Load formula");
    loadFormulaButton.onClick = [this]() {
        using namespace formula::processor;
        eventHub->publish(EventType::loadFormulaRequest, metadata);
    };
    addAndMakeVisible(loadFormulaButton);

}


void formula::gui::FormulaDetailsPanel::setFormula(formula::processor::FormulaMetadata metadata) {
    nameLabel.setText(metadata[formula::processor::FormulaMetadataKeys::name], NotificationType::sendNotificationAsync);
    codePreviewEditor->loadContent(metadata[formula::processor::FormulaMetadataKeys::source]);
    descriptionLabel.setText(metadata[formula::processor::FormulaMetadataKeys::description], NotificationType::sendNotificationAsync);
    this->metadata = metadata;

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
}

