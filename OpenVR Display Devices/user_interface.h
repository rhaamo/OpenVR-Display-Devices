#pragma once

#include "config.h"
#include "configuration.h"
#include "notifications.h"
#include "BatteryChecker.h"

#include <windows.h>

#include <imgui.h>
#include <openvr.h>

#include <stdio.h>
#include <memory>
#include <vector>
#include <any>

void BuildMainWindow(bool runningInOverlay);
void RequestImmediateRedraw();
void ShowVersionLine();
void Show_StatusTable();
void Show_Settings();
