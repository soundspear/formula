/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_STORAGE_COMPILERSTORAGE_INCLUDED
#define FORMULA_STORAGE_COMPILERSTORAGE_INCLUDED

#include <string>
#include <fstream>

#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <processor/PluginState.hpp>
#include <storage/LocalStorage.hpp>

namespace formula::storage {
    /**
     * Manages the storage for the compiler
     */
	class CompilerStorage : public LocalStorage {
	public:
		struct CompiledLibraryPath {
			std::string twoChannelsLibraryPath;
			std::string singleChannelLibrariesPaths[2];
		};

		CompilerStorage();
		std::string createSourceFile(
                const std::string& compilationId,
                const std::string& sourceCode);
        void copyLibFormula();
		void deleteSourceFile(const std::string& compilationId);
		CompiledLibraryPath getLibraryPath(const std::string& compilationId);
		void deleteLibrary(const std::string& compilationId);

        static std::string createCompilationId();
	private:
		const std::string sourceFileSuffix = ".c";
		const std::string twoChannelsLibrarySuffix = ".2ch.formula";
		const std::string singleChannelLibrariesPaths[2] = { ".1ch.left.formula", ".1ch.right.formula" };
        std::string libFormulaCode;
	};
}

#endif // FORMULA_STORAGE_COMPILERSTORAGE_INCLUDED