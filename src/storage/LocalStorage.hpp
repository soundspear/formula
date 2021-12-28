#ifndef FORMULA_STORAGE_LOCAL_STORAGE_INCLUDED
#define FORMULA_STORAGE_LOCAL_STORAGE_INCLUDED

#include <boost/filesystem.hpp>

namespace formula::storage {
	class LocalStorage {
	public:
		LocalStorage() {
			storageFolder =
#ifdef _WIN32
#	pragma warning( push )
#	pragma warning(disable: 4996)
				boost::filesystem::path(std::getenv("APPDATA")) / "formula";
#	pragma warning( pop ) 
#else
				boost::filesystem::path(std::getenv("HOME")) / ".formula";
#endif
		}

	protected:
		boost::filesystem::path storageFolder;
	};
}

#endif // FORMULA_STORAGE_LOCAL_STORAGE_INCLUDED