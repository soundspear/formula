![Formula](assets/logo.svg)
---
Formula is an open-source VST & AU plugin to create custom audio effects inside your DAW.

**👨‍💻 For developers**: Live code and test your effects right inside your DAW.

**🎸For end-users**: Browse and use hundreds of effects made by developers.

## Quick start

#### Using binary setup
Download and install the latest release for Windows, macOS or Linux [here](https://github.com/soundspear/formula/releases).

#### Build from source code
1. Run `configure-windows.ps1`, `configure-mac.sh` or `configure-linux.sh` depending on your platform. This will download and locally install all our dependencies (JUCE, boost, vcpkg, cpprestskd).
2. If you have an IDE that supports CMake (Visual Studio 17+, CLion...) just open the repository folder and build the default target.
3. Else, run `cmake . -G Ninja -DVCPKG_TARGET_TRIPLET=x64-windows-static -DCMAKE_TOOLCHAIN_FILE="./vcpkg/scripts/buildsystems/vcpkg.cmake"`.
Replace `x64-windows-static` by `x64-osx` or `arm64-osx` depending on your platform.
5. Finally, run `cmake --build .`

---

![Demo](/assets/demo.gif)

## Contribute

If you like this project and you would like to contribute, you are welcome!
The workflow is as follows:

- Fork the repository
- Create a branch based on `develop`
- Code your features in that branch
- Rebase from upstream develop
- Submit a pull request on our `develop` branch
