#pragma once

#include "config.h"
#include "configuration.h"

// Includes (order matters for GL3W/GLFW3)
#include "GL/gl3w.h"
#include <GLFW/glfw3.h>
#include <openvr.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <windows.h>
#include <stdio.h>
#include <direct.h>

#include <stdexcept>
#include <iostream>
#include <thread>

#include "droid_sans_font.h"
#include "user_interface.h"
