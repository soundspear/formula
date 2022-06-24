#ifndef FORMULA_STORAGE_LOCALINDEX_INCLUDED
#define FORMULA_STORAGE_LOCALINDEX_INCLUDED

#include <fstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <processor/PluginState.hpp>
#include <storage/LocalStorage.hpp>
#include <events/EventHub.hpp>
#include <processor/FormulaMetadata.hpp>

namespace formula::storage {
	class LocalIndexIterator {
	public:
		LocalIndexIterator() = delete;

		operator formula::processor::FormulaMetadata() const
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

	class LocalIndex : public LocalStorage {
	public:
		LocalIndex(
			const std::shared_ptr<formula::processor::PluginState>& pluginStateRef
		);
        static std::string serializeMetadata(const formula::processor::FormulaMetadata& metadata);
        static formula::processor::FormulaMetadata deserializeMetadata(const std::string& metadata);

		void refreshIndex();
		void saveIndex();
		void saveCurrentFormulaToIndex();
		void addFormulaToIndex(formula::processor::FormulaMetadata& metadata, bool overrideExisting);
		void deleteFormula(std::string name);

		LocalIndexIterator begin();
		LocalIndexIterator end();
	private:
		boost::property_tree::ptree index;
		boost::filesystem::path indexPath;

		std::shared_ptr<formula::events::EventHub> eventHub;
		std::shared_ptr<formula::processor::PluginState> pluginState;
	};
}

#endif // FORMULA_STORAGE_LOCALINDEX_INCLUDED