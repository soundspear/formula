/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_PROCESSOR_FORMULAVERSION_INCLUDED
#define FORMULA_PROCESSOR_FORMULAVERSION_INCLUDED

#include "src/storage/CompilerStorage.hpp"

namespace formula::processor::library {
    class FormulaVersion {
    public:
        FormulaVersion() : isMono(false) {}
        virtual ~FormulaVersion() = default;
        virtual bool tryLoad(const formula::storage::CompilerStorage::CompiledLibraryPath& path) = 0;
        virtual void unload() = 0;
        virtual void processBlock(
                AudioBuffer<float>& buffer,
                float dryWet, float inGain, float outGain,
                double bpm, double bar,
                float* userKnobs, float* userSwitches,
                double sampleRate,
                int *debugStackIdx,
                char **debugStack,
                int debugStackSize
        ) = 0;
    protected:
        std::atomic<bool> isMono;
    };
}
#endif //FORMULA_PROCESSOR_FORMULAVERSION_INCLUDED
