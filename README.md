![Formula](assets/logo.svg)
---
Formula is an open-source VST & AU plugin to create custom audio effects inside your DAW.

**👨‍💻 For developers**: Live code and test your effects right inside your DAW. Share your effects with the community.

**🎸For end-users**: Browse and use hundreds of bundled open-source effects made by developers from around the world.

## Quick start

Full documentation and audio programming 101 are available in the [User Manual](https://github.com/soundspear/formula/blob/release/assets/manual/Formula-User-Manual.pdf).

#### Install with binary setup
Download and install the latest release for Windows, macOS or Linux [here](https://github.com/soundspear/formula/releases).

#### Build from source code
1. Run `configure-windows.ps1`, `configure-mac.sh` or `configure-linux.sh` depending on your platform. This will download and locally install all our dependencies (JUCE, boost, vcpkg, cpprestskd).
2. Use CMake to generate the project: `cmake . -G Ninja -DVCPKG_TARGET_TRIPLET=x64-windows-static -DCMAKE_TOOLCHAIN_FILE="./vcpkg/scripts/buildsystems/vcpkg.cmake" -DFORMULA_STANDALONE=1`.
Replace `x64-windows-static` by `x64-osx` or `arm64-osx` depending on your platform?
3. Finally, run `cmake --build .`

---

![Demo](/assets/demo.webp)

## Contribute

Contributions are more than welcome, either code improvements or sharing your formulas! 

The workflow is as follows:

- Fork the repository
- Create a branch based on `develop`
  - To share a formula: export the Formula from the Saved Files tab, copy it to the `formulas/` folder at the root of the repository
- Submit a pull request on our `develop` branch
