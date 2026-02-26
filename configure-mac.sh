#!/bin/zsh

set -e

JuceVersion="7.0.5"
VCPKGVersion="2026.01.16"

echo "Installing CMake"
brew install cmake
brew install ninja

echo "Installing vcpkg"
git clone https://github.com/microsoft/vcpkg
cd vcpkg
git reset --hard "$VCPKGVersion"
cd ..
./vcpkg/bootstrap-vcpkg.sh

echo "Installing Boost"
BOOST_PACKAGES=(
	"boost-any"
	"boost-assign"
	"boost-date-time"
	"boost-dll"
	"boost-filesystem"
	"boost-format"
	"boost-iostreams"
	"boost-lexical-cast"
	"boost-process"
	"boost-preprocessor"
	"boost-property-tree"
	"boost-signals2"
	"boost-uuid"
)
for PACKAGE in $BOOST_PACKAGES; do
	./vcpkg/vcpkg install $PACKAGE --triplet "x64-osx"
	./vcpkg/vcpkg install $PACKAGE --triplet "arm64-osx"
done

echo "Installing cpprestsdk"
./vcpkg/vcpkg install cpprestsdk --triplet "x64-osx"
./vcpkg/vcpkg install cpprestsdk --triplet "arm64-osx"

echo "Installing JUCE"
if [[ ! -d "JUCE" ]]; then
	curl -s -L "https://github.com/juce-framework/JUCE/archive/refs/tags/$JuceVersion.tar.gz" | tar xvz -C .
	mv JUCE-$JuceVersion JUCE
fi
