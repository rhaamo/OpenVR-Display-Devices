#pragma once

// lol
#define UNICODE

// To avoid windows.h to redefine min() and max()
#define NOMINMAX

// lol2: Somehow needs to be "first" to avoid boost::asio and winsock/winsock2 conflict...
#include <winsock2.h>

#define OPENVR_APPLICATION_KEY "ottpossum.OvrDisplayDevices"
#define OPENVR_APPLICATION_NAME "OpenVR Display Devices"
#define APPLICATION_CONFIG_DIR "OpenVR-Display-Devices"

#define UPDATE_INTERVAL 0.5
#define MINIMIZED_MAX_FPS 60.0f

#define BAT_OK IM_COL32(52, 224, 69, 255)
#define BAT_WARN IM_COL32(224, 173, 52, 255)
#define BAT_LOW IM_COL32(209, 31, 31, 255)
#define TRACKING_OK IM_COL32(52, 224, 69, 255)
#define TRACKING_WAIT IM_COL32(224, 173, 52, 255)
#define TRACKING_KO IM_COL32(209, 31, 31, 255)

#define APP_VERSION_MAJOR 0
#define APP_VERSION_MINOR 2
