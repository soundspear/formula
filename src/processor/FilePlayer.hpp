/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_PROCESSOR_FILEPLAYER_INCLUDED
#define FORMULA_PROCESSOR_FILEPLAYER_INCLUDED

#include <JuceHeader.h>

namespace formula::processor {
    /**
     * Loads and plays an audio file through a buffer
     */
    class FilePlayer {
    public:
        FilePlayer();
        bool loadFile(String& path);
        void prepare(double sampleRate, int maximumExpectedSamplesPerBlock);
        void prepare();
        void getNextBlock(AudioBuffer<float> &buffer);
        juce::String& getCurrentPath();
        juce::String getWildcardForAllFormats();

    private:
        juce::AudioFormatManager formatManager;
        std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
        juce::AudioTransportSource transportSource;
        juce::String currentPath;
        double currentSampleRate;
        double currentSamplesPerBlock;
    };
}


#endif //FORMULA_PROCESSOR_FILEPLAYER_INCLUDED
