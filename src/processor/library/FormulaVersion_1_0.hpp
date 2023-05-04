/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_PROCESSOR_FORMULAVERSION_1_0_INCLUDED
#define FORMULA_PROCESSOR_FORMULAVERSION_1_0_INCLUDED

#include "boost/dll/import.hpp"

#include <processor/library/FormulaVersion.hpp>

namespace formula::processor::library {
    /**
     * First version of libformula
     */
    class FormulaVersion_1_0 : public FormulaVersion {
    private:
        typedef void (ProcessBlockMono)(float* in, int numSamples, float sampleRate,
                                        const float* knobs, const float* switches,
                                        float wet, float inGain, float outGain,
                                        int* debug_idx, char** debug_stack, int debug_stack_size);

        typedef void (ProcessBlockStereo)(float* const* in, int numSamples, float sampleRate,
                                          const float* knobs, const float* switches,
                                          float wet, float inGain, float outGain,
                                          int* debug_idx, char** debug_stack, int debug_stack_size);

    public:
        bool tryLoad(const formula::storage::CompilerStorage::CompiledLibraryPath& path) override {

            boost::dll::shared_library formula;

            isMono = boost::filesystem::exists(path.singleChannelLibrariesPaths[0]);

            if (isMono) {
                for (int channel = 0; channel < 2; channel++) {
                    singleChannelLibraries[channel] = boost::dll::shared_library();
                    singleChannelLibraries[channel].load(path.singleChannelLibrariesPaths[channel]);
                    if (!singleChannelLibraries[channel].has(processBlockMonoSymbolName)) {
                        singleChannelLibraries[channel].unload();
                        return false;
                    }
                    singleChannelEntrypoints[channel] = singleChannelLibraries[channel].get<ProcessBlockMono>(processBlockMonoSymbolName);
                }
            }
            else {
                twoChannelsLibrary = boost::dll::shared_library();
                twoChannelsLibrary.load(path.twoChannelsLibraryPath);
                if (!twoChannelsLibrary.has(processBlockStereoSymbolName)) {
                    twoChannelsLibrary.unload();
                    return false;
                }
                twoChannelsEntrypoint = twoChannelsLibrary.get<ProcessBlockStereo>(processBlockStereoSymbolName);
            }

            return true;
        }

        void unload() override {
            if (isMono) {
                for (auto & singleChannelLibrary : singleChannelLibraries) {
                    if (singleChannelLibrary.is_loaded()) {
                        singleChannelLibrary.unload();
                    }
                }
            }
            else {
                if (twoChannelsLibrary.is_loaded()) {
                    twoChannelsLibrary.unload();
                }
            }
        }

        void processBlock(
                AudioBuffer<float>& buffer,
                float dryWet, float inGain, float outGain,
                [[maybe_unused]] double bpm, [[maybe_unused]] double bar,
                float* userKnobs, float* userSwitches,
                double sampleRate,
                int *debugStackIdx,
                char **debugStack,
                int debugStackSize
        ) override {

            float* const* writePointers = buffer.getArrayOfWritePointers();
            int numSamples = buffer.getNumSamples();
            int numChannels = buffer.getNumChannels();

            if (isMono) {
                for (int channel = 0; channel < numChannels; channel++) {
                    singleChannelEntrypoints[channel](
                            writePointers[channel],
                            numSamples,
                            static_cast<float>(sampleRate),
                            userKnobs, userSwitches,
                            dryWet, inGain, outGain,
                            debugStackIdx, debugStack, debugStackSize
                    );
                }
            }
            else {
                if (buffer.getNumChannels() != 2) return;
                twoChannelsEntrypoint(
                        writePointers,
                        numSamples,
                        static_cast<float>(sampleRate),
                        userKnobs, userSwitches,
                        dryWet, inGain, outGain,
                        debugStackIdx, debugStack, debugStackSize
                );
            }
        }

    private:
        boost::dll::shared_library singleChannelLibraries[2];
        boost::dll::shared_library twoChannelsLibrary;
        ProcessBlockMono* singleChannelEntrypoints[2];
        ProcessBlockStereo* twoChannelsEntrypoint;

        static constexpr char processBlockMonoSymbolName[] = "process_block_mono";
        static constexpr char processBlockStereoSymbolName[] = "process_block_stereo";
    };
}

#endif //FORMULA_PROCESSOR_FORMULAVERSION_1_0_INCLUDED
