#include "config.h"
#include "configuration.h"
#include "droid_sans_font.h"
#include "notifications.h"
#include "user_interface.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <openvr.h>
#include <direct.h>
#include <chrono>
#include <thread>
#include <iostream>



AppConfig application_configuration;

static GLFWwindow *glfwWindow = nullptr;
static GLuint fboHandle, fboTextureHandle = 0;
static int fboTextureWidth, fboTextureHeight = 0;
static vr::VROverlayHandle_t overlayMainHandle, overlayThumbnailHandle = 0;
static char textBuf[0x400];
static char cwd[MAX_PATH];

double lastFrameStartTime = glfwGetTime();

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
//extern "C" __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
//extern "C" __declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001;

//extern void ActivateMultipleDrivers();

void CreateConsole() {
	static bool created = false;
	if (!created) {
		AllocConsole();
		FILE *file = nullptr;
		freopen_s(&file, "CONIN$", "r", stdin);
		freopen_s(&file, "CONOUT$", "w", stdout);
		freopen_s(&file, "CONOUT$", "w", stderr);
		created = true;
	}
}

void GLFWErrorCallback(int error, const char *description) {
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void InitVR() {
	auto initError = vr::VRInitError_None;
	vr::VR_Init(&initError, vr::VRApplication_Other);
	if (initError != vr::VRInitError_None) {
		auto error = vr::VR_GetVRInitErrorAsEnglishDescription(initError);
		throw std::runtime_error("OpenVR error :" + std::string(error));
	}

	if (!vr::VR_IsInterfaceVersionValid(vr::IVRSystem_Version)) {
		throw std::runtime_error("OpenVR error: Outdated IVRSystem_Version");
	} else if (!vr::VR_IsInterfaceVersionValid(vr::IVRSettings_Version)) {
		throw std::runtime_error("OpenVR error: Outdated IVRSettings_Version");
	} else if (!vr::VR_IsInterfaceVersionValid(vr::IVROverlay_Version)) {
		throw std::runtime_error("OpenVR error: Outdated IVROverlay_Version");
	}

	//ActivateMultipleDrivers();
}

void CreateGLFWWindow() {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, false);

	fboTextureWidth = 1600;
	fboTextureHeight = 800;

	glfwWindow = glfwCreateWindow(fboTextureWidth, fboTextureHeight, OPENVR_APPLICATION_NAME/*"OpenVR-Display-Devices"*/, NULL, NULL);
	if (!glfwWindow) {
		throw std::runtime_error("Failed to create window");
	}

	glfwMakeContextCurrent(glfwWindow);
	glfwSwapInterval(1);
	gl3wInit();

	// Minimize the window on start if enabled
//#ifndef _DEBUG
	 //glfwIconifyWindow(glfwWindow);
//#endif

	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.IniFilename = nullptr;
	io.Fonts->AddFontFromMemoryCompressedTTF(DroidSans_compressed_data, DroidSans_compressed_size, 24.0f);

	ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	ImGui::StyleColorsDark();

	glGenTextures(1, &fboTextureHandle);
	glBindTexture(GL_TEXTURE_2D, fboTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, fboTextureWidth, fboTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenFramebuffers(1, &fboHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fboTextureHandle, 0);

	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw std::runtime_error("OpenGL framebuffer incomplete");
	}
}

extern BatteryChecker batteryChecker;
std::string overlayThumbnailCurrent = "";

void UpdateOverlayIcon() {
	if (!vr::VROverlay() || !overlayThumbnailHandle)
	{
		return;
	}

	std::string iconPath = cwd;

	if (batteryChecker.hmdCharging)
	{
		if (batteryChecker.hmdCharge > 80)
		{
			iconPath += "\\icons\\c100.png";
		}
		else if (batteryChecker.hmdCharge > 60)
		{
			iconPath += "\\icons\\c80.png";
		}
		else if (batteryChecker.hmdCharge > 40)
		{
			iconPath += "\\icons\\c60.png";
		}
		else if (batteryChecker.hmdCharge > 20)
		{
			iconPath += "\\icons\\c40.png";
		}
		else
		{
			iconPath += "\\icons\\c20.png";
		}
	}
	else
	{
		if (batteryChecker.hmdCharge > 80) 
		{
			iconPath += "\\icons\\100.png";
		} 
		else if (batteryChecker.hmdCharge > 60) 
		{
			iconPath += "\\icons\\80.png";
		} 
		else if (batteryChecker.hmdCharge > 40) 
		{
			iconPath += "\\icons\\60.png";
		} 
		else if (batteryChecker.hmdCharge > 20) 
		{
			iconPath += "\\icons\\40.png";
		} 
		else 
		{
			iconPath += "\\icons\\20.png";
		}
	}

	if (overlayThumbnailCurrent == iconPath.c_str())
	{// Unchanged...
		return;
	}

	//printf("icon : %s\n", iconPath.c_str());
	vr::VROverlay()->ClearOverlayTexture(overlayThumbnailHandle);
	vr::VROverlay()->SetOverlayFromFile(overlayThumbnailHandle, iconPath.c_str());
	overlayThumbnailCurrent = iconPath.c_str();
}

void TryCreateVROverlay() {
	if (overlayMainHandle || !vr::VROverlay()) {
		return;
	}

	vr::VROverlayError error = vr::VROverlay()->CreateDashboardOverlay(
		OPENVR_APPLICATION_KEY, OPENVR_APPLICATION_NAME, &overlayMainHandle, &overlayThumbnailHandle
	);

	if (error == vr::VROverlayError_KeyInUse) {
		throw std::runtime_error("Another instance of OpenVR Display Devices is already running");
	} else if (error != vr::VROverlayError_None) {
		throw std::runtime_error("Error creating VR overlay: " + std::string(vr::VROverlay()->GetOverlayErrorNameFromEnum(error)));
	}

	vr::VROverlay()->SetOverlayWidthInMeters(overlayMainHandle, 3.0f);
	vr::VROverlay()->SetOverlayInputMethod(overlayMainHandle, vr::VROverlayInputMethod_Mouse);
	vr::VROverlay()->SetOverlayFlag(overlayMainHandle, vr::VROverlayFlags_SendVRDiscreteScrollEvents, true);
	UpdateOverlayIcon();
}

/*void ActivateMultipleDrivers() {
	vr::EVRSettingsError vrSettingsError;
	bool enabled = vr::VRSettings()->GetBool(vr::k_pch_SteamVR_Section, vr::k_pch_SteamVR_ActivateMultipleDrivers_Bool, &vrSettingsError);

	if (vrSettingsError != vr::VRSettingsError_None) {
		std::string err = "Could not read \"" + std::string(vr::k_pch_SteamVR_ActivateMultipleDrivers_Bool) + "\" setting: "
			+ vr::VRSettings()->GetSettingsErrorNameFromEnum(vrSettingsError);

		throw std::runtime_error(err);
	}

	if (!enabled) {
		vr::VRSettings()->SetBool(vr::k_pch_SteamVR_Section, vr::k_pch_SteamVR_ActivateMultipleDrivers_Bool, true, &vrSettingsError);
		if (vrSettingsError != vr::VRSettingsError_None) {
			std::string err = "Could not set \"" + std::string(vr::k_pch_SteamVR_ActivateMultipleDrivers_Bool) + "\" setting: "
				+ vr::VRSettings()->GetSettingsErrorNameFromEnum(vrSettingsError);

			throw std::runtime_error(err);
		}

		std::cerr << "Enabled \"" << vr::k_pch_SteamVR_ActivateMultipleDrivers_Bool << "\" setting" << std::endl;
	} else {
		std::cerr << "\"" << vr::k_pch_SteamVR_ActivateMultipleDrivers_Bool << "\" setting previously enabled" << std::endl;
	}
}*/

void RunLoop() {
	while (!glfwWindowShouldClose(glfwWindow)) {
		TryCreateVROverlay();

		bool dashboardVisible = false;
		int width, height;
		glfwGetFramebufferSize(glfwWindow, &width, &height);
		const bool windowVisible = (width > 0 && height > 0);

		if (overlayMainHandle && vr::VROverlay()) {
			auto &io = ImGui::GetIO();
			dashboardVisible = vr::VROverlay()->IsActiveDashboardOverlay(overlayMainHandle) || vr::VROverlay()->IsDashboardVisible();

			vr::VREvent_t vrEvent;
			while (vr::VROverlay()->PollNextOverlayEvent(overlayMainHandle, &vrEvent, sizeof(vrEvent))) {
				switch (vrEvent.eventType) {
					case vr::VREvent_MouseMove:
						io.AddMousePosEvent(vrEvent.data.mouse.x, vrEvent.data.mouse.y);
						break;
					case vr::VREvent_MouseButtonDown:
						io.AddMouseButtonEvent(vrEvent.data.mouse.button == vr::VRMouseButton_Left ? 0 : 1, true);
						break;
					case vr::VREvent_MouseButtonUp:
						io.AddMouseButtonEvent(vrEvent.data.mouse.button == vr::VRMouseButton_Left ? 0 : 1, false);
						break;
					case vr::VREvent_ScrollDiscrete:
					{
						float x = vrEvent.data.scroll.xdelta * 360.0f * 8.0f;
						float y = vrEvent.data.scroll.ydelta * 360.0f * 8.0f;
						io.AddMouseWheelEvent(x, y);
						break;
					}
					case vr::VREvent_Quit:
						return;
				}
			}

			//if (windowVisible || dashboardVisible) 
			{
				auto &io = ImGui::GetIO();
				io.DisplaySize = ImVec2((float)fboTextureWidth, (float)fboTextureHeight);
				io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

				io.ConfigFlags = io.ConfigFlags & ~ImGuiConfigFlags_NoMouseCursorChange;
				if (dashboardVisible) {
					io.ConfigFlags = io.ConfigFlags | ImGuiConfigFlags_NoMouseCursorChange;
				}
				
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();

				BuildMainWindow(dashboardVisible);

				ImGui::Render();

				glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
				glViewport(0, 0, fboTextureWidth, fboTextureHeight);
				glClearColor(0, 0, 0, 1);
				glClear(GL_COLOR_BUFFER_BIT);

				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				if (width && height) {
					glBindFramebuffer(GL_READ_FRAMEBUFFER, fboHandle);
					glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
					glfwSwapBuffers(glfwWindow);
				}

				if (dashboardVisible) {
					vr::Texture_t vrTex;
					vrTex.eType = vr::TextureType_OpenGL;
					//vrTex.eColorSpace = vr::ColorSpace_Auto;
					vrTex.eColorSpace = vr::ColorSpace_Linear;

					vrTex.handle = (void *)
#if defined _WIN64 || defined _LP64
					(uint64_t)
#endif
						fboTextureHandle;
					

					//vrTex.handle = reinterpret_cast<void *>(static_cast<uintptr_t>(fboTextureHandle));

					vr::VROverlay()->ClearOverlayTexture(overlayMainHandle);
					vr::VROverlay()->SetOverlayTexture(overlayMainHandle, &vrTex);
					//vr::VROverlay()->SetOverlayAlpha(overlayMainHandle, 1.0f);

					//vr::VROverlay()->ClearOverlayTexture(overlayMainHandle);
					//vr::VROverlay()->SetOverlayFromFile(overlayMainHandle, overlayThumbnailCurrent.c_str());

					/*vr::Texture_t texture = {(void *)(uintptr_t)&fboTextureHandle, vr::TextureType_OpenGL, vr::ColorSpace_Auto};
					vr::VROverlay()->SetOverlayTexture(overlayMainHandle, &texture);*/

					vr::HmdVector2_t mouseScale = { (float)fboTextureWidth, (float)fboTextureHeight };
					vr::VROverlay()->SetOverlayMouseScale(overlayMainHandle, &mouseScale);

					//printf("Overlay set to texture %I64u. Resolution %i x %i.\n", (uint64_t)vrTex.handle, fboTextureWidth, fboTextureHeight);
				}
			}

			if (vr::VROverlay()->IsDashboardVisible()) {
				UpdateOverlayIcon();
			}

			const double dashboardInterval = 1.0 / 90.0; // fps
			double waitEventsTimeout = std::max(UPDATE_INTERVAL, dashboardInterval);

			if (dashboardVisible && waitEventsTimeout > dashboardInterval) {
				waitEventsTimeout = dashboardInterval;
			}

			glfwWaitEventsTimeout(waitEventsTimeout);

			// If we are minimized rendering won't limit our frame rate so we need to do it ourselves
			if (glfwGetWindowAttrib(glfwWindow, GLFW_ICONIFIED)) {
				double targetFrameTime = 1 / MINIMIZED_MAX_FPS;
				double waitTime = targetFrameTime - (glfwGetTime() - lastFrameStartTime);
				if (waitTime > 0) {
					std::this_thread::sleep_for(std::chrono::duration<double>(waitTime));
				}
				lastFrameStartTime += targetFrameTime;
			}
		}
	}
}

static void HandleCommandLine(LPWSTR lpCmdLine) {
	if (lstrcmp(lpCmdLine, L"-openvrpath") == 0) {
		auto vrErr = vr::VRInitError_None;
		vr::VR_Init(&vrErr, vr::VRApplication_Utility);
		if (vrErr == vr::VRInitError_None) {
			char cruntimePath[MAX_PATH] = { 0 };
			unsigned int pathLen;
			vr::VR_GetRuntimePath(cruntimePath, MAX_PATH, &pathLen);

			printf("%s", cruntimePath);
			vr::VR_Shutdown();
			exit(0);
		}
		fprintf(stderr, "Failed to initialize OpenVR: %s\n", vr::VR_GetVRInitErrorAsEnglishDescription(vrErr));
		vr::VR_Shutdown();
		exit(-2);
	} else if (lstrcmp(lpCmdLine, L"-installmanifest") == 0) {
		auto vrErr = vr::VRInitError_None;
		vr::VR_Init(&vrErr, vr::VRApplication_Utility);
		if (vrErr == vr::VRInitError_None) {
			if (vr::VRApplications()->IsApplicationInstalled(OPENVR_APPLICATION_KEY)) {
				char oldWd[MAX_PATH] = { 0 };
				auto vrAppErr = vr::VRApplicationError_None;
				vr::VRApplications()->GetApplicationPropertyString(OPENVR_APPLICATION_KEY, vr::VRApplicationProperty_WorkingDirectory_String, oldWd, MAX_PATH, &vrAppErr);
				if (vrAppErr != vr::VRApplicationError_None) {
					fprintf(stderr, "Failed to get old working dir, skipping removal: %s\n", vr::VRApplications()->GetApplicationsErrorNameFromEnum(vrAppErr));
				} else {
					std::string manifestPath = oldWd;
					manifestPath += "\\manifest.vrmanifest";
					std::cout << "Removing old manifest path: " << manifestPath << std::endl;
					vr::VRApplications()->RemoveApplicationManifest(manifestPath.c_str());
				}
			}
			std::string manifestPath = cwd;
			manifestPath += "\\manifest.vrmanifest";
			std::cout << "Adding manifest path: " << manifestPath << std::endl;
			auto vrAppErr = vr::VRApplications()->AddApplicationManifest(manifestPath.c_str());
			if (vrAppErr != vr::VRApplicationError_None) {
				fprintf(stderr, "Failed to add manifest: %s\n", vr::VRApplications()->GetApplicationsErrorNameFromEnum(vrAppErr));
			} else {
				vr::VRApplications()->SetApplicationAutoLaunch(OPENVR_APPLICATION_KEY, true);
			}
			vr::VR_Shutdown();
			exit(-2);
		}
		fprintf(stderr, "Failed to initialize OpenVR: %s\n", vr::VR_GetVRInitErrorAsEnglishDescription(vrErr));
		vr::VR_Shutdown();
		exit(-2);
	} else if (lstrcmp(lpCmdLine, L"-removemanifest") == 0) {
		auto vrErr = vr::VRInitError_None;
		vr::VR_Init(&vrErr, vr::VRApplication_Utility);
		if (vrErr == vr::VRInitError_None) {
			if (vr::VRApplications()->IsApplicationInstalled(OPENVR_APPLICATION_KEY)) {
				std::string manifestPath = cwd;
				manifestPath += "\\manifest.vrmanifest";
				std::cout << "Removing manifest path: " << manifestPath << std::endl;
				vr::VRApplications()->RemoveApplicationManifest(manifestPath.c_str());
			}
			vr::VR_Shutdown();
			exit(0);
		}
		fprintf(stderr, "Failed to initialize OpenVR: %s\n", vr::VR_GetVRInitErrorAsEnglishDescription(vrErr));
		vr::VR_Shutdown();
		exit(-2);
	}
}

// Main func
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	_getcwd(cwd, MAX_PATH);
	HandleCommandLine(lpCmdLine);

#ifdef _DEBUG
	CreateConsole();
#endif

	if (!glfwInit()) {
		MessageBox(nullptr, L"Failed to initialize GLFW", L"", 0);
		return 0;
	}

	glfwSetErrorCallback(GLFWErrorCallback);

	try {
		InitVR();
		CreateGLFWWindow();

		// Load config then do post-config setup if needed
		loadOrInitConfiguration(application_configuration);
		if (application_configuration.notificationsSound) {
			enableSoundNotifications();
		}
		if (application_configuration.notificationsXsOverlay) {
			enableXsOverlayNotifications();
		}

		if (!application_configuration.loaded) {
			throw std::runtime_error("Config isn't loaded");
		}
		RunLoop();

		if (application_configuration.notificationsSound) {
			disableSoundNotifications();
		}
		if (application_configuration.notificationsXsOverlay) {
			disableXsOverlayNotifications();
		}

		vr::VR_Shutdown();

		if (fboHandle) {
			glDeleteFramebuffers(1, &fboHandle);
		}

		if (fboTextureHandle) {
			glDeleteTextures(1, &fboTextureHandle);
		}

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	} catch (std::runtime_error &e) {
		std::cerr << "Runtime error: " << e.what() << std::endl;
		wchar_t message[1024];
		swprintf(message, 1024, L"%hs", e.what());
		MessageBox(nullptr, message, L"Runtime Error", 0);
	}

	if (glfwWindow) {
		glfwDestroyWindow(glfwWindow);
	}

	glfwTerminate();

	return 0;
}