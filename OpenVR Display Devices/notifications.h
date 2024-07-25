#pragma once

#include "config.h"
#include "configuration.h"
#include <codecvt>

#define XSOVERLAY_WS "ws://localhost:42070/?client=OVRDisplayDevices"

#include <wintoastlib.h>

//#include <boost/beast/core.hpp>
//#include <boost/beast/websocket.hpp>
//#include <boost/asio/strand.hpp>

void enableWindowsNotifications();
void disableWindowsNotifications();
void enableXsOverlayNotifications();
void disableXsOverlayNotifications();
void sendNotification(std::string &title, std::string& msg);
void sendXsOverlayNotification(std::string& title, std::string& msg);
void sendNativeNotification(std::string& title, std::string& msg);
