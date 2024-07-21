/*
    Copyright (c) 2024 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef FORMULA_GUI_FORMULALISTTABBASE_INCLUDED
#define FORMULA_GUI_FORMULALISTTABBASE_INCLUDED

#include <JuceHeader.h>

#include <processor/FormulaMetadata.hpp>
#include <storage/LocalIndex.hpp>

namespace formula::gui {
    class FormulaListTabBase : public TableListBoxModel, public Component {
    public:
        explicit FormulaListTabBase(const std::shared_ptr<formula::storage::LocalIndex>& indexRef);
        int getNumRows() override;
        void paintRowBackground(Graphics& g, int rowNumber, int, int, bool rowIsSelected) override;
        void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool) override;
        void sortOrderChanged(int newSortColumnId, bool isForwards) override;
        void visibilityChanged() override;

        virtual void refreshData() = 0;
    protected:
        enum ColumnsIds {
            source = 1, name = 2, created = 3, lastModified = 4, description = 5
        };

        std::vector<formula::processor::FormulaMetadata> data;
        TableListBox table;
        std::shared_ptr<formula::storage::LocalIndex> index;
    };
}


#endif // FORMULA_GUI_FORMULALISTTABBASE_INCLUDED
