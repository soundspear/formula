#!/bin/bash

JuceVersion="7.0.5"
LLVMVersion="11"
BoostVersion="1.74"

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

echo "Installing Prerequisites"
apt-get install curl tar gzip build-essential -y
apt-get install libasound2-dev libfreetype6-dev libfontconfig1-dev xclip libcurl4-openssl-dev  -y
apt-get install webkit2gtk-4.0 libgtk-3-dev -y 

echo "Installing CMake"
apt-get install cmake ninja-build pkg-config -y

echo "Installing LLVM"
apt-get install llvm-$LLVMVersion clang-$LLVMVersion -y

echo "Installing Boost"
apt-get install libboost$BoostVersion-dev -y
BOOST_PACKAGES=(
	"boost-date-time"
	"boost-filesystem"
)
for PACKAGE in "${BOOST_PACKAGES[@]}"
do
	apt-get install lib$PACKAGE$BoostVersion-dev -y
done

: '
BoostVersion_=$(echo boost_$BoostVersion | tr '.' '_')
https://boostorg.jfrog.io/artifactory/main/release/$BoostVersion/source/boost_$(echo $BoostVersion | tr '.' '_').tar.gz
tar xzfv $BoostVersion_.tar.gz
cd $BoostVersion_
./bootstrap.sh --prefix=/usr/local/
./b2 && ./b2 install
'

echo "Installing JUCE"
if [[ ! -d "JUCE" ]]; then
	curl -s -L "https://github.com/juce-framework/JUCE/archive/refs/tags/$JuceVersion.tar.gz" | tar xvz -C .
	mv JUCE-$JuceVersion JUCE
  chmod -R 777 JUCE
fi

echo "Installing cpprestsdk"
apt-get install libcpprest-dev -y