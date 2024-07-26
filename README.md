# OpenVR Display Devices

<img src="./screenshot_standalone.png" width="600px"> <img src="./screenshot_steamvr.png" width="400px">

## WTF is that

This is a SteamVR Overlay app (and a "standalone" window) that will list all your connected OpenVR devices informations such as: Model, vendor, tracking type, serial number, battery status and tracking state.

It can send notifications of low battery (<20%) and warn (<50%) through native windows toasts or XSOverlay.

The configuration file is located in: `%appdata%\OpenVR-Display-Devices\` the `app.ini` file.

## Build

- Clone the repository recursively to get the submodules
- Alternatively in the CLI after cloning: `git submodule init --update` iirc
- Load the project in Visual Studio 2022
- Build it
- ???

To create an installer:
- Download latest NSIS thingy
- Open it, click the link to compile a script
- Load the "installer/installer.nsi" file
- Compile and execute
- done

## TODO

- Try it more
- VR Runtime path (xxx.exe -runtimepath) doesn't return anything ???

## License

MIT License

Some code has been used from https://github.com/hyblocker/OpenVR-SpaceCalibrator for the main ImGUI/glfw/opengl window code.
