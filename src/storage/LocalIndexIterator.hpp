//    Copyright (c) 2024 Soundspear.
//    Distributed under the Boost Software License, Version 1.0.
//    See http://www.boost.org/LICENSE_1_0.txt

#ifndef FORMULA_STORAGE_LOCALINDEXITERATOR_INCLUDED
#define FORMULA_STORAGE_LOCALINDEXITERATOR_INCLUDED

#include <processor/FormulaMetadata.hpp>

namespace formula::storage {
    class LocalIndexIterator {
    public:
        LocalIndexIterator() = delete;

        explicit operator formula::processor::FormulaMetadata() const
        {
            formula::processor::FormulaMetadata metadata;

            metadata[formula::processor::FormulaMetadataKeys::name] = iterator->first;
            auto metadataTree = iterator->second;
            for (auto it = metadataTree.begin(); it != metadataTree.end(); ++it) {
                metadata[it->first] = it->second.data();
            }
            return metadata;
        }

        LocalIndexIterator& operator ++() {
            ++iterator;
            return (*this);
        }

        bool operator==(const LocalIndexIterator& other) const {
            return iterator == other.iterator;
        }

        bool operator!=(const LocalIndexIterator& other) const {
            return !(*this == other);
        }

    private:
        LocalIndexIterator(boost::property_tree::ptree::const_iterator ptree_iterator)
                : iterator(ptree_iterator) {
        }

        boost::property_tree::ptree::const_iterator iterator;

        friend class LocalIndex;
    };
}

#endif // FORMULA_STORAGE_LOCALINDEXITERATOR_INCLUDED
