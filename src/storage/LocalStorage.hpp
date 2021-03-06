/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_STORAGE_LOCALSTORAGE_INCLUDED
#define FORMULA_STORAGE_LOCALSTORAGE_INCLUDED

#include <boost/filesystem.hpp>

namespace formula::storage {
    /**
     * Base class for a local storage manager
     */
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

            if (!boost::filesystem::exists(storageFolder))
            {
                boost::filesystem::create_directories(storageFolder);
            }
		}

	protected:
		boost::filesystem::path storageFolder;
	};
}

#endif // FORMULA_STORAGE_LOCALSTORAGE_INCLUDED