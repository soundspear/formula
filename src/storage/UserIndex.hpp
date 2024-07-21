//    Copyright (c) 2024 Soundspear.
//    Distributed under the Boost Software License, Version 1.0.
//    See http://www.boost.org/LICENSE_1_0.txt

#ifndef FORMULA_STORAGE_USERINDEX_INCLUDED
#define FORMULA_STORAGE_USERINDEX_INCLUDED

#include <storage/LocalIndex.hpp>

namespace formula::storage {
    class UserIndex : public LocalIndex {
    public:
        UserIndex(
            const std::shared_ptr<formula::processor::PluginState>& pluginStateRef
        );

        void loadIndex() override;
        void saveIndex() override;
    private:
        boost::filesystem::path indexPath;
    };
}

#endif // FORMULA_STORAGE_USERINDEX_INCLUDED
