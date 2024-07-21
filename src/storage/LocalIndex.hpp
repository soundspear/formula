/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_STORAGE_LOCALINDEX_INCLUDED
#define FORMULA_STORAGE_LOCALINDEX_INCLUDED

#include <fstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <processor/PluginState.hpp>
#include <storage/LocalStorage.hpp>
#include <storage/LocalIndexIterator.hpp>
#include <events/EventHub.hpp>
#include <processor/FormulaMetadata.hpp>

namespace formula::storage {
    /**
     * Manages the formulas that are saved locally
     */
	class LocalIndex : public LocalStorage {
	public:
        static std::string serializeMetadata(const formula::processor::FormulaMetadata& metadata);
        static formula::processor::FormulaMetadata deserializeMetadata(const std::string& metadata);

        void saveCurrentFormulaToIndex(const std::shared_ptr<formula::processor::PluginState>& pluginStateRef);
		void addFormulaToIndex(formula::processor::FormulaMetadata& metadata, bool overrideExisting);
		void deleteFormula(std::string name);

		LocalIndexIterator begin();
		LocalIndexIterator end();

        virtual void loadIndex() = 0;
        virtual void saveIndex() = 0;
	protected:
		boost::property_tree::ptree index;

		std::shared_ptr<formula::events::EventHub> eventHub;
	};
}

#endif // FORMULA_STORAGE_LOCALINDEX_INCLUDED