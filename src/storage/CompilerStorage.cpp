#include "CompilerStorage.hpp"

formula::storage::CompilerStorage::CompilerStorage()
    : formula::storage::LocalStorage()
{
}

std::string formula::storage::CompilerStorage::createCompilationId()
{
    const auto uuid = boost::uuids::random_generator()();
    return std::string(boost::uuids::to_string(uuid));
}

std::string formula::storage::CompilerStorage::createSourceFile(
        const std::string& compilationId,
        const std::string& sourceCode)
{
    const auto path = storageFolder / (compilationId + sourceFileSuffix);
    boost::filesystem::ofstream file(path);
    file << sourceCode;
    file.close();
    return path.string();
}

void formula::storage::CompilerStorage::deleteSourceFile(const std::string& compilationId)
{
    const auto path = storageFolder / (compilationId + sourceFileSuffix);
    boost::filesystem::remove(path);
}

formula::storage::CompilerStorage::CompiledLibraryPath 
formula::storage::CompilerStorage::getLibraryPath(const std::string& compilationId)
{
    return CompiledLibraryPath {
        .twoChannelsLibraryPath = (storageFolder / (compilationId + twoChannelsLibrarySuffix)).string(),
        .singleChannelLibrariesPaths = {
            (storageFolder / (compilationId + singleChannelLibrariesPaths[0])).string(),
            (storageFolder / (compilationId + singleChannelLibrariesPaths[1])).string(),
        }
    };
}

void formula::storage::CompilerStorage::deleteLibrary(const std::string& compilationId) {
    const auto libraryPaths = getLibraryPath(compilationId);

    boost::filesystem::remove(libraryPaths.twoChannelsLibraryPath);
    boost::filesystem::remove(libraryPaths.singleChannelLibrariesPaths[0]);
    boost::filesystem::remove(libraryPaths.singleChannelLibrariesPaths[1]);

#if defined (_WIN32)
    for (auto extension : { "exp", "lib" }) {
        boost::filesystem::remove(
            libraryPaths.twoChannelsLibraryPath
            .substr(0, libraryPaths.twoChannelsLibraryPath.size() - 7)
            + extension);
        boost::filesystem::remove(
            libraryPaths.singleChannelLibrariesPaths[0]
            .substr(0, libraryPaths.singleChannelLibrariesPaths[0].size() - 7)
            + extension);
        boost::filesystem::remove(
            libraryPaths.singleChannelLibrariesPaths[1]
            .substr(0, libraryPaths.singleChannelLibrariesPaths[1].size() - 7)
            + extension);
    }
#endif
}