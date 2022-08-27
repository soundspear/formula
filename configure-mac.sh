#!/bin/zsh

JuceVersion="7.0.1"
LLVMVersion="11"
BoostVersion="1.78" # 1.74 not available through brew
OpenSSLVersion="1.1"

echo "Installing CMake"
brew install cmake
brew install ninja

which -s clang
if [[ $? != 0 ]] ; then
  echo "Installing LLVM"
  brew install llvm@$LLVMVersion
fi

echo "Installing vcpkg"
git clone https://github.com/microsoft/vcpkg
cd vcpkg
git reset --hard "2022.02.02"
cd ..
./vcpkg/bootstrap-vcpkg.sh

echo "Installing Boost"
BOOST_PACKAGES=(
	"boost-any"
	"boost-assign"
	"boost-date-time"
	"boost-dll"
	"boost-filesystem"
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
