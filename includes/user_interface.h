#pragma once

// lol
#define UNICODE

#include "config.h"

#include <imgui.h>
#include <openvr.h>

#include <windows.h>
#include <stdio.h>
#include <memory>
#include <vector>
#include <any>

void BuildMainWindow(bool runningInOverlay);
void RequestImmediateRedraw();
void ShowVersionLine();
void Show_StatusTable();
void Show_Settings();
