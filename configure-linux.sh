#!/bin/bash

JuceVersion="7.0.5"
LLVMVersion="18"
BoostVersion="1.84.0"

set -e

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

echo "Updating package index"
apt-get update -y

echo "Installing Prerequisites"
apt-get install curl tar gzip build-essential -y
apt-get install libasound2-dev libfreetype-dev libfontconfig1-dev xclip libcurl4-openssl-dev -y
apt-get install libwebkit2gtk-4.1-dev libgtk-3-dev -y
apt-get install xorg-dev -y

echo "Installing CMake"
apt-get install cmake ninja-build pkg-config -y

echo "Installing LLVM"
apt-get install llvm-$LLVMVersion clang-$LLVMVersion -y

echo "Installing Boost"
BoostVersion_=$(echo boost_$BoostVersion | tr '.' '_')
wget "https://github.com/boostorg/boost/releases/download/boost-$BoostVersion/boost-$BoostVersion-b2-nodocs.tar.gz"
tar xzfv "boost-$BoostVersion-b2-nodocs.tar.gz"
cd "boost-$BoostVersion"
./bootstrap.sh --prefix=/usr/local/
./b2 link=static runtime-link=static --with-date_time	--with-filesystem --with-iostreams
./b2 install
cd ..

echo "Installing JUCE"
if [[ ! -d "JUCE" ]]; then
	curl -s -L "https://github.com/juce-framework/JUCE/archive/refs/tags/$JuceVersion.tar.gz" | tar xvz -C .
	mv JUCE-$JuceVersion JUCE
  chmod -R 777 JUCE
fi

echo "Installing cpprestsdk"
apt-get install libcpprest-dev -y