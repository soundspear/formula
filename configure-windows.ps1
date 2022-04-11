#Requires -RunAsAdministrator

$JuceVersion="6.1.4"
$LLVMVersion="11.1.0"
$OpenSSLVersion="1.1.1.1300"
$BoostVersion="1.74"
$CMakeVersion="3.22.1"

if (Get-Command -Name choco -ErrorAction SilentlyContinue) {
  Write-Host "Chocolatey package manager already installed"
  choco upgrade chocolatey -y
} else {
  Set-ExecutionPolicy Bypass -Scope Process -Force
  [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
  iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
}
refreshenv
Write-Host
Write-Host "Installing CMake $CMakeVersion"
choco install cmake --version $CMakeVersion --installargs 'ADD_CMAKE_TO_PATH=User' -y
choco install ninja
Write-Host
Write-Host "Installing OpenSSL $OpenSSLVersion"
choco install openssl --version $OpenSSLVersion -y
Write-Host
Write-Host "Installing Boost $BoostVersion"
choco install boost-msvc-14.2 --version $BoostVersion -y
Write-Host
Write-Host "Installing LLVM $LLVMVersion"
choco install llvm --version $LLVMVersion -y
Write-Host
Write-Host "Downloading JUCE $JuceVersion"
Invoke-WebRequest -uri "https://github.com/juce-framework/JUCE/archive/refs/tags/$JuceVersion.zip" -Method "GET" -Outfile "juce.zip"
Expand-Archive "juce.zip" -DestinationPath "." -Force
Rename-Item -Path "JUCE-$JuceVersion" -NewName "JUCE"
Remove-Item "juce.zip"
Write-Host "Downloading Tinycc"
Invoke-WebRequest -uri "http://download.savannah.gnu.org/releases/tinycc/tcc-0.9.27-win64-bin.zip" -Method "GET" -Outfile "tinycc.zip"
Expand-Archive "tinycc.zip" -DestinationPath "$Env:Programfiles" -Force
Remove-Item "tinycc.zip"

Write-Host "Installing vcpkg"
git clone https://github.com/Microsoft/vcpkg.git --depth 1
.\vcpkg\bootstrap-vcpkg.bat
.\vcpkg\vcpkg install --triplet x64-windows-static cpprestsdk
.\vcpkg\vcpkg integrate install

Write-Host "All done."