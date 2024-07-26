#include "notifications.h"

Webchaussette wsClient(XSOVERLAY_WS_HOST, XSOVERLAY_WS_PORT, XSOVERLAY_WS_QUERY);

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

    std::string title = "Test notification";
    std::string content = "nya~ :3";

    sendNativeNotification(title, content);
    std::cout << "Enabled toaster notifications" << std::endl;
}

void disableWindowsNotifications() {
    // Nothing to do here
}

void enableXsOverlayNotifications() {
    wsClient.Start();

    std::string title = "Test notification";
    std::string content = "nya~ :3";
    sendXsOverlayNotification(title, content);
    std::cout << "Enabled XSOverlay notifications" << std::endl;
}

std::string buildXsOverlayJson(std::string title, std::string content) {
    nlohmann::json jNotification = {
        {"title", title},
        {"content", content},
        {"timeout", 3},
        {"height", "150"},
        {"opacity", 1},
        {"volume", 1},
        {"audioPath", "default"},
        {"icon", nullptr},
        {"sourceApp", ""},
        {"type", 1},
        {"useBase64Icon", false}
    };
    nlohmann::json jWrapper = {
        {"sender", "OVRDisplayDevices"},
        {"target", "xsoverlay"},
        {"command", "SendNotification"},
        {"jsonData", jNotification.dump()},
        {"rawData", nullptr}
    };
    return jWrapper.dump();
}

void disableXsOverlayNotifications() {
    wsClient.Stop();
}

std::wstring charToWstring(const char* src) {
    return std::wstring(src, src + strlen(src));
}

void sendNativeNotification(std::string &title, std::string &msg) {
    WinToastLib::WinToastTemplate::AudioOption audioOptions = WinToastLib::WinToastTemplate::AudioOption::Default;

    WinToastLib::WinToastTemplate templ(WinToastLib::WinToastTemplate::Text02);
    // Title
    templ.setTextField(charToWstring(title.c_str()), WinToastLib::WinToastTemplate::FirstLine);
    // Content
    templ.setAttributionText(charToWstring(msg.c_str()));
    templ.setAudioOption(audioOptions);
    // templ.setExpiration(5);
    if (WinToastLib::WinToast::instance()->showToast(templ, new CustomHandler()) < 0) {
        throw std::runtime_error("Woops, cannot toast :(");
    }
}

void sendXsOverlayNotification(std::string &title, std::string& msg) {
    std::string jmsg = buildXsOverlayJson(title, msg);
    wsClient.Send(jmsg);
}

void sendNotification(std::string &title, std::string &msg) {
    if (application_configuration.notificationsWindows) {
        std::cout << "SN: " << title << std::endl;
        sendNativeNotification(title, msg);
    }
    if (application_configuration.notificationsXsOverlay) {
        sendXsOverlayNotification(title, msg);
    }
}
