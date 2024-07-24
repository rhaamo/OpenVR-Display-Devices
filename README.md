# OpenVR Display Devices

<img src="./screenshot_standalone.png" width="600px"> <img src="./screenshot_steamvr.png" width="400px">

## WTF is that

This is standalone and SteamVR Overlay app that will list all your connected OpenVR devices informations.

Model, vendor, tracking type, serial number, battery status and tracking state are shown.

## Build

- I developped it with msys2 and VSCode
- Packages required on msys2 (ucrt environment): `base-devel mingw-w64-ucrt-x86_64-clang mingw-w64-ucrt-x86_64-gdb mingw-w64-x86_64-cmake mingw-w64-x86_64-git mingw-w64-ucrt-x86_64-libconfig mingw-w64-ucrt-x86_64-cmake`
- `git clone --recursive ` the repository to get the third party libs
- mkdir build
- cd build
- cmake .. && make
- and uh, maybe it will work for you

## TODO

- Try it more
- Implement notifications through XSOverlay
- Add ability to be registered as an app by SteamVR

## License

MIT License

Some code has been used from https://github.com/hyblocker/OpenVR-SpaceCalibrator for the main ImGUI/glfw/opengl window code.
