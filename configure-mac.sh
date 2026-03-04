#!/bin/zsh

set -e

JuceVersion="8.0.4"
VCPKGVersion="2026.01.16"

echo "Installing dependencies"
brew install cmake ninja pkg-config

echo "Installing vcpkg"
if [[ ! -d "vcpkg" ]]; then
	git clone --filter=blob:none https://github.com/microsoft/vcpkg.git
	git -C vcpkg checkout "$VCPKGVersion"
fi
./vcpkg/bootstrap-vcpkg.sh

echo "Installing JUCE"
if [[ ! -d "JUCE" ]]; then
	curl -s -L "https://github.com/juce-framework/JUCE/archive/refs/tags/$JuceVersion.tar.gz" | tar xvz -C .
	mv JUCE-$JuceVersion JUCE
fi
