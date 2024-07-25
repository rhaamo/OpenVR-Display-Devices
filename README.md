# OpenVR Display Devices

<img src="./screenshot_standalone.png" width="600px"> <img src="./screenshot_steamvr.png" width="400px">

## WTF is that

This is standalone and SteamVR Overlay app that will list all your connected OpenVR devices informations.

Model, vendor, tracking type, serial number, battery status and tracking state are shown.

## Build

- Clone the repository recursively to get the submodules
- Alternatively in the CLI after cloning: `git submodule init --update` iirc
- Load the project in Visual Studio 2022
- Build it
- ???

## TODO

- Try it more
- Implement notifications through XSOverlay
- Implement actual notifications dispatch for low and warn
- Add ability to be registered as an app by SteamVR (implemented, to be tested)

## License

MIT License

Some code has been used from https://github.com/hyblocker/OpenVR-SpaceCalibrator for the main ImGUI/glfw/opengl window code.
