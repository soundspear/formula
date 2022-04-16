#!/bin/zsh

# Use the flag "--cross-compile <target architecture>" to compile for another
# architecture than the current one
# <target architecture> must be either:
#	-  arm64: compiling for M1 while on Intel
#	-  x64: compiling for Intel while on M1
if [[ "$1" == "--cross-compile" ]]; then
  echo "Cross compiling for $2 architecture"
  CROSS_COMPILE=1
  
  if [[ "$2" == "arm64" ]]; then
	TARGET_HOMEBREW_ARCH="arm64_"
  elif [[ "$2" == "x64" ]]; then
	TARGET_HOMEBREW_ARCH=""
  else
	echo "Unkown target architecture"
	echo "Usage: --cross-compile <target architecture>"
	exit 1;
  fi
fi

homebrew_xcomp () {
	brew uninstall --ignore-dependencies "$1" 2> /dev/null || true
	# Homebrew does not allow installing a package for a different architecture
	# We must download the bottle and install it manually
	response=$(brew fetch --force --bottle-tag="${TARGET_HOMEBREW_ARCH}big_sur" "$1" | grep "Downloaded to")
	package_name=$(echo -n "$response" | cut -d " " -f 3)
	brew install "$package_name"
}

homebrew_native () {
	brew install $1
}

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
[ -z "$CROSS_COMPILE" ] && homebrew_native openssl@$OpenSSLVersion || homebrew_xcomp openssl@$OpenSSLVersion

echo "Installing Boost"
[ -z "$CROSS_COMPILE" ] && homebrew_native boost@$BoostVersion || homebrew_xcomp boost@$BoostVersion 

echo "Installing cpprestsdk"
[ -z "$CROSS_COMPILE" ] && homebrew_native cpprestsdk || homebrew_xcomp cpprestsdk 

echo "Installing JUCE"
if [[ ! -d "JUCE" ]]; then
	curl -s -L "https://github.com/juce-framework/JUCE/archive/refs/tags/$JuceVersion.tar.gz" | tar xvz -C .
	mv JUCE-$JuceVersion JUCE
fi
