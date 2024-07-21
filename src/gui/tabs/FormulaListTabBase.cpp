/*
    Copyright (c) 2024 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
 */

#include "FormulaListTabBase.hpp"

using FormulaMetadata = formula::processor::FormulaMetadata;
using FormulaMetadataKeys = formula::processor::FormulaMetadataKeys;

formula::gui::FormulaListTabBase::FormulaListTabBase(const std::shared_ptr<formula::storage::LocalIndex> &indexRef)
    : index(indexRef) {
}

int formula::gui::FormulaListTabBase::getNumRows()
{
    return static_cast<int>(data.size());
}

void formula::gui::FormulaListTabBase::paintRowBackground(Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected)
{
    auto alternateColour = getLookAndFeel().findColour(ListBox::backgroundColourId)
        .interpolatedWith(getLookAndFeel().findColour(ListBox::textColourId), 0.03f);
    if (rowIsSelected)
        g.fillAll(Colours::lightblue);
    else if (rowNumber % 2)
        g.fillAll(alternateColour);
}

void formula::gui::FormulaListTabBase::paintCell(Graphics& g, int rowNumber, int columnId,
    int width, int height, bool /*rowIsSelected*/)
{
    if (rowNumber >= static_cast<int>(data.size())) {
        return;
    }
    g.setColour(getLookAndFeel().findColour(ListBox::textColourId));
    g.setFont(Font());

    auto row = data[static_cast<unsigned int>(rowNumber)];
    String text;

    switch (columnId) {
        case ColumnsIds::name:
            text = row[formula::processor::FormulaMetadataKeys::name];
        break;
        case ColumnsIds::created:
            text = row[formula::processor::FormulaMetadataKeys::created];
        break;
        case ColumnsIds::lastModified:
            text = row[formula::processor::FormulaMetadataKeys::lastModified];
        break;
        case ColumnsIds::description:
            text = row[formula::processor::FormulaMetadataKeys::description];
        break;
        default:
            return;
    }

    g.drawText(text, 2, 0, width - 4, height, Justification::centredLeft, true);

    g.setColour(getLookAndFeel().findColour(ListBox::backgroundColourId));
    g.fillRect(width - 1, 0, 1, height);
}

void formula::gui::FormulaListTabBase::sortOrderChanged(int newSortColumnId, bool isForwards)
{
    std::function<bool(formula::processor::FormulaMetadata, formula::processor::FormulaMetadata)> predicate;
    switch (newSortColumnId) {
        case ColumnsIds::name:
            predicate = [](FormulaMetadata a, FormulaMetadata b) {
                return a[FormulaMetadataKeys::name] < b[FormulaMetadataKeys::name];
        };
        break;
        case ColumnsIds::created:
            predicate = [](FormulaMetadata a, FormulaMetadata b) {
                const auto& timeA = boost::posix_time::time_from_string(a[FormulaMetadataKeys::created]);
                const auto& timeB = boost::posix_time::time_from_string(b[FormulaMetadataKeys::created]);
                return timeA < timeB;
            };
        break;
        case ColumnsIds::lastModified:
            predicate = [](FormulaMetadata a, FormulaMetadata b) {
                const auto& timeA = boost::posix_time::time_from_string(a[FormulaMetadataKeys::lastModified]);
                const auto& timeB = boost::posix_time::time_from_string(b[FormulaMetadataKeys::lastModified]);
                return timeA < timeB;
        };
        break;
        case ColumnsIds::description:
            predicate = [](FormulaMetadata a, FormulaMetadata b) {
                return a[FormulaMetadataKeys::description] < b[FormulaMetadataKeys::description];
        };
        break;
        default:
            return;
    }
    if (isForwards) {
        std::sort(data.begin(), data.end(), predicate);
    }
    else {
        std::sort(data.rbegin(), data.rend(), predicate);
    }

    table.updateContent();
}

void formula::gui::FormulaListTabBase::visibilityChanged()
{
    if (isVisible()) {
        index->loadIndex();
        refreshData();
        table.updateContent();
    }
}

