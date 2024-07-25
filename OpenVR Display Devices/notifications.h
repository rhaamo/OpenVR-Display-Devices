#pragma once

// lol
#define UNICODE

#include "config.h"
#include "configuration.h"

#define XSOVERLAY_WS "ws://localhost:42070/?client=OVRDisplayDevices"

#include <wintoastlib.h>

//#include <boost/beast/core.hpp>
//#include <boost/beast/websocket.hpp>
//#include <boost/asio/strand.hpp>

void enableWindowsNotifications();
void disableWindowsNotifications();
void enableXsOverlayNotifications();
void disableXsOverlayNotifications();
void sendNotification(char* title, char* msg);
void sendXsOverlayNotification(char* title, char* msg);
void sendNativeNotification(char* title, char* msg);
