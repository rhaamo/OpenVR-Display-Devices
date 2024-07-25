#include "notifications.h"

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

void enableWindowsNotifications() {
    if (!WinToastLib::WinToast::isCompatible()) {
        throw std::runtime_error("System incompatible :(");
    }
    WinToastLib::WinToast::instance()->setAppName(L"" OPENVR_APPLICATION_NAME);
    const auto aumi = WinToastLib::WinToast::configureAUMI(L"ottpossum", L"" OPENVR_APPLICATION_NAME, L"farts", L"42");
    WinToastLib::WinToast::instance()->setAppUserModelId(aumi);
    if (!WinToastLib::WinToast::instance()->initialize()) {
        throw std::runtime_error("Woops, error when initializing the WinToaster");
    }

    sendNativeNotification("Test notification", "nya~ :3");
}

void disableWindowsNotifications() {
    // Nothing to do here
}

void enableXsOverlayNotifications() {
    // Todo
    sendNotification("Test notification", "nya~ :3");
}

void disableXsOverlayNotifications() {
}

std::wstring charToWstring(const char* src) {
    return std::wstring(src, src + strlen(src));
}

void sendNativeNotification(char* title, char* msg) {
    WinToastLib::WinToastTemplate::AudioOption audioOptions = WinToastLib::WinToastTemplate::AudioOption::Default;

    WinToastLib::WinToastTemplate templ(WinToastLib::WinToastTemplate::Text02);
    // Title
    templ.setTextField(charToWstring(title), WinToastLib::WinToastTemplate::FirstLine);
    // Content
    templ.setAttributionText(charToWstring(msg));
    templ.setAudioOption(audioOptions);
    templ.setExpiration(5);
    if (WinToastLib::WinToast::instance()->showToast(templ, new CustomHandler()) < 0) {
        throw std::runtime_error("Woops, cannot toast :(");
    }
}

void sendXsOverlayNotification(char* title, char* msg) {
}

void sendNotification(char* title, char* msg) {
    if (application_configuration.notificationsWindows) {
        sendNativeNotification(title, msg);
    }
    if (application_configuration.notificationsXsOverlay) {
        sendXsOverlayNotification(title, msg);
    }
}
