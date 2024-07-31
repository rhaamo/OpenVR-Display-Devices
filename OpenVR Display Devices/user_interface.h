#pragma once

#include "config.h"
#include "configuration.h"
#include "notifications.h"
#include "battery_checker.h"

#include <imgui.h>
#include <openvr.h>

#include <stdio.h>
#include <memory>
#include <vector>
#include <any>

void BuildMainWindow(bool runningInOverlay);
void ShowVersionLine();
void Show_StatusTable();
void Show_Settings();
