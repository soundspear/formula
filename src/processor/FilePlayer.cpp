/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#include "FilePlayer.hpp"

#include <memory>

formula::processor::FilePlayer::FilePlayer() {
    std::vector<juce::AudioFormat*> formats = {
        new juce::AiffAudioFormat(),
#if defined __APPLE__
        new juce::CoreAudioFormat(),
#endif
        new juce::FlacAudioFormat(),
        new juce::MP3AudioFormat(),
        new juce::OggVorbisAudioFormat(),
        new juce::WavAudioFormat(),
    };
    for (auto* format : formats) {
        // AudioFormatManager manages the lifetime of the pointers
        formatManager.registerFormat(format, false);
    }
}

bool formula::processor::FilePlayer::loadFile(String& path) {
    try {
        if (readerSource) {
            readerSource.release();
        }
        currentPath = path;
        auto file = File(currentPath);
        auto *reader = formatManager.createReaderFor(file);
        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate, 2);
        transportSource.setPosition(0);
        prepare();
    } catch (std::exception&) {
        return false;
    }
    return true;
}

void formula::processor::FilePlayer::prepare(double sampleRate, int maximumExpectedSamplesPerBlock) {
    currentSampleRate = sampleRate;
    currentSamplesPerBlock = maximumExpectedSamplesPerBlock;
    if (readerSource == nullptr) {
        return;
    }

    transportSource.stop();
    transportSource.prepareToPlay(maximumExpectedSamplesPerBlock, sampleRate);
    readerSource->prepareToPlay(maximumExpectedSamplesPerBlock, sampleRate);
    transportSource.start();
}

void formula::processor::FilePlayer::prepare() {
    prepare(currentSampleRate, currentSamplesPerBlock);
}

void formula::processor::FilePlayer::getNextBlock(AudioBuffer<float> &buffer) {
    if (!transportSource.isPlaying()) {
        return;
    }
    transportSource.getNextAudioBlock(AudioSourceChannelInfo(buffer));

    // TransportSource.SetLooping does not work
    if (transportSource.getNextReadPosition() >= transportSource.getTotalLength()) {
        transportSource.setPosition(0);
        transportSource.start();
    }
}

juce::String &formula::processor::FilePlayer::getCurrentPath() {
    return currentPath;
}

juce::String formula::processor::FilePlayer::getWildcardForAllFormats() {
    return formatManager.getWildcardForAllFormats();
}
