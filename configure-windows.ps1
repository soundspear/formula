#Requires -RunAsAdministrator

$ErrorActionPreference = "Stop"

$JuceVersion="8.0.4"
$LLVMVersion="11.1.0"
$OpenSSLVersion="1.1.1.1300"
$BoostVersion="1.84.0"
$VCPKGVersion="2026.01.16"

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
Write-Host "Installing OpenSSL $OpenSSLVersion"
choco install openssl --version $OpenSSLVersion -y
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

Write-Host "Installing Boost $BoostVersion"
$BoostVersionUnderscore = $BoostVersion -replace '\.', '_'
$BoostInstaller = "boost_${BoostVersionUnderscore}-msvc-14.3-64.exe"
$BoostUrl = "https://sourceforge.net/projects/boost/files/boost-binaries/$BoostVersion/$BoostInstaller/download"
Invoke-WebRequest -Uri $BoostUrl -OutFile $BoostInstaller -UserAgent "Wget"
Start-Process -Wait -FilePath ".\$BoostInstaller" -ArgumentList "/SILENT","/DIR=C:\local\boost_${BoostVersionUnderscore}"
Remove-Item $BoostInstaller

Write-Host "Installing vcpkg"
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
git reset --hard "$VCPKGVersion"
cd ..
.\vcpkg\bootstrap-vcpkg.bat

Write-Host "Installing cpprestsdk via vcpkg (classic mode)"
Rename-Item vcpkg.json vcpkg.json.bak
.\vcpkg\vcpkg install --triplet x64-windows-static cpprestsdk
.\vcpkg\vcpkg integrate install
Rename-Item vcpkg.json.bak vcpkg.json

Write-Host "All done."