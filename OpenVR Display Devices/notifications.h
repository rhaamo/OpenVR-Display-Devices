#pragma once

#include "config.h"
#include "configuration.h"
#include "webchaussette.h"
#include <codecvt>
#include <nlohmann/json.hpp>

#define XSOVERLAY_WS "ws://localhost:42070/?client=OVRDisplayDevices"
#define XSOVERLAY_WS_HOST "localhost"
#define XSOVERLAY_WS_PORT 42070
#define XSOVERLAY_WS_QUERY "?client=OVRDisplayDevices"

#include <wintoastlib.h>

class CustomHandler : public WinToastLib::IWinToastHandler {
public:
    void toastActivated() const {
        //std::wcout << L"The user clicked in this toast" << std::endl;
    }

    void toastActivated(int actionIndex) const {
        //std::wcout << L"The user clicked on action #" << actionIndex << std::endl;
    }

    void toastDismissed(WinToastDismissalReason state) const {
        switch (state) {
        case UserCanceled:
            //std::wcout << L"The user dismissed this toast" << std::endl;
            break;
        case TimedOut:
            //std::wcout << L"The toast has timed out" << std::endl;
            break;
        case ApplicationHidden:
            //std::wcout << L"The application hid the toast using ToastNotifier.hide()" << std::endl;
            break;
        default:
            //std::wcout << L"Toast not activated" << std::endl;
            break;
        }
    }

    void toastFailed() const {
        throw std::runtime_error("Error showing current toast");
    }
};

void enableWindowsNotifications();
void disableWindowsNotifications();
void enableXsOverlayNotifications();
void disableXsOverlayNotifications();
void sendNotification(std::string &title, std::string& msg);
void sendXsOverlayNotification(std::string& title, std::string& msg);
void sendNativeNotification(std::string& title, std::string& msg);
std::string buildXsOverlayJson(std::string title, std::string content);
