#!/bin/zsh

JuceVersion="6.1.4"
LLVMVersion="11"
BoostVersion="1.78" # 1.74 not available through brew
OpenSSLVersion="1.1"

which -s brew
if [[ $? != 0 ]] ; then
  echo "Installing homebrew"
  /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"
fi

echo "Installing CMake"
brew install cmake
brew install ninja

which -s clang
if [[ $? != 0 ]] ; then
  echo "Installing LLVM"
  brew install llvm@$LLVMVersion
fi

echo "Installing OpenSSL"
brew install openssl@$OpenSSLVersion

echo "Installing Boost"
brew install boost@$BoostVersion

echo "Installing JUCE"
curl -s -L "https://github.com/juce-framework/JUCE/archive/refs/tags/$JuceVersion.tar.gz" | tar xvz -C .
mv JUCE-$JuceVersion JUCE

echo "Installing cpprestsdk"
brew install cpprestsdk