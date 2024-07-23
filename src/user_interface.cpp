#include "user_interface.h"

static bool runningInOverlay;
static const ImGuiWindowFlags bareWindowFlags =
	ImGuiWindowFlags_NoTitleBar |
	ImGuiWindowFlags_NoResize |
	ImGuiWindowFlags_NoMove |
	ImGuiWindowFlags_NoScrollbar |
	ImGuiWindowFlags_NoScrollWithMouse |
	ImGuiWindowFlags_NoCollapse;

bool cfgAlertLowBattery = true;
float cfgBatteryWarn = 50;
float cfgBatteryLow = 20;
bool cfgLoaded = false;

void BuildMainWindow(bool runningInOverlay_) {
    runningInOverlay = runningInOverlay_;
    
    auto &io = ImGui::GetIO();

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);

    if (!ImGui::Begin(OPENVR_APPLICATION_NAME, nullptr, bareWindowFlags)) {
        ImGui::End();
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImGui::GetStyleColorVec4(ImGuiCol_Button));

    // load config


    static ImVec2 windowSize = ImGui::GetWindowSize();
    // Top part
    ImGui::BeginChild("statusTable", { windowSize.x, windowSize.y - 100 }, ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoSavedSettings);
    ImGui::SetWindowFontScale(0.8f);
    Show_StatusTable();
    ImGui::SetWindowFontScale(1.0f);
    ImGui::EndChild();

    // Bottom part
    ImGui::BeginChild("settingsTable", { windowSize.x, 100 }, ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoSavedSettings);
    Show_Settings();
    ImGui::EndChild();

    //BuildTabs();
    ShowVersionLine();

    ImGui::PopStyleColor();
    ImGui::End();
}

void ShowVersionLine() {
    ImGui::SetNextWindowPos(ImVec2(10.0f, ImGui::GetWindowHeight() - ImGui::GetFrameHeightWithSpacing()));
	if (!ImGui::BeginChild("bottom line", ImVec2(ImGui::GetWindowWidth() - 20.0f, ImGui::GetFrameHeightWithSpacing() * 2), false)) {
		ImGui::EndChild();
		return;
	}
	ImGui::Text(OPENVR_APPLICATION_NAME " v" APP_VERSION);
	if (runningInOverlay) {
		ImGui::SameLine();
		ImGui::Text("- close VR overlay to use mouse");
	}
	ImGui::EndChild();
}

void Show_StatusTable() {
    const ImGuiTableFlags flags = ImGuiTableFlags_RowBg & ~ImGuiTableFlags_Sortable | ImGuiTableFlags_HighlightHoveredColumn | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerH;
    if (ImGui::BeginTable("OvrDD_StatusTable", 7, flags)) {
        // Columns
        ImGui::TableSetupColumn("[ID]", ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableSetupColumn("Class",ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_WidthFixed, 200.0f);
        ImGui::TableSetupColumn("Role", ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_WidthFixed, 200.0f);
        ImGui::TableSetupColumn("Battery %", ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Model", ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_WidthFixed, 300.0f);
        ImGui::TableSetupColumn("Serial", ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_WidthStretch, -1);
        ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_WidthStretch, -1);
        ImGui::TableHeadersRow();

        for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
            auto deviceClass = vr::VRSystem()->GetTrackedDeviceClass(i);
            vr::ETrackedPropertyError pError;

            if (deviceClass != vr::TrackedDeviceClass_Invalid) {
                // VR Device ID: i
                // Class
                char deviceClassString[20];
                switch (deviceClass) {
                    case 0:
                        strcpy(deviceClassString, "Invalid");
                        break;
                    case 1:
                        strcpy(deviceClassString, "HMD");
                        break;
                    case 2:
                        strcpy(deviceClassString, "Controller");
                        break;
                    case 3:
                        strcpy(deviceClassString, "Tracker"); // GenericTracker
                        break;
                    case 4:
                        strcpy(deviceClassString, "Lighthouse"); // TrackingReference
                        break;
                    case 5:
                        strcpy(deviceClassString, "DisplayRedirect");
                        break;
                    default:
                        strcpy(deviceClassString, "Unknown");
                        break;
                }

                // Role
                char roleBuffer[vr::k_unMaxPropertyStringSize] = "<unknown>";
                vr::VRSystem()->GetStringTrackedDeviceProperty(i, vr::Prop_ControllerType_String, roleBuffer, vr::k_unMaxPropertyStringSize, &pError);

                if (pError != vr::TrackedProp_Success && pError != vr::TrackedProp_UnknownProperty) {
                    wchar_t message[1024];
                    swprintf(message, 1024, L"Could not get Prop_ControllerType of device %i: %s", i, vr::VRSystem()->GetPropErrorNameFromEnum(pError));
                    MessageBox(nullptr, message, L"Runtime Error", 0);
				}

                // Serial number
                char serialBuffer[vr::k_unMaxPropertyStringSize] = "<ERROR>";
                vr::VRSystem()->GetStringTrackedDeviceProperty(i, vr::Prop_SerialNumber_String, serialBuffer, vr::k_unMaxPropertyStringSize, &pError);
                if (pError != vr::TrackedProp_Success && pError != vr::TrackedProp_UnknownProperty) {
                    wchar_t message[1024];
                    swprintf(message, 1024, L"Could not get Prop_SerialNumber of device %i: %s", i, vr::VRSystem()->GetPropErrorNameFromEnum(pError));
                    MessageBox(nullptr, message, L"Runtime Error", 0);
				}

                // Battery %
                float deviceBatteryPercent = vr::VRSystem()->GetFloatTrackedDeviceProperty(i, vr::Prop_DeviceBatteryPercentage_Float, &pError);
                if (pError != vr::TrackedProp_Success && pError != vr::TrackedProp_UnknownProperty) {
                    wchar_t message[1024];
                    swprintf(message, 1024, L"Could not get Prop_DeviceBatteryPercentage of device %i: %s", i, vr::VRSystem()->GetPropErrorNameFromEnum(pError));
                    MessageBox(nullptr, message, L"Runtime Error", 0);
				}

                bool deviceIsCharging = vr::VRSystem()->GetBoolTrackedDeviceProperty(i, vr::Prop_DeviceIsCharging_Bool, &pError);
                if (pError != vr::TrackedProp_Success && pError != vr::TrackedProp_UnknownProperty) {
                    wchar_t message[1024];
                    swprintf(message, 1024, L"Could not get Prop_DeviceIsCharging of device %i: %s", i, vr::VRSystem()->GetPropErrorNameFromEnum(pError));
                    MessageBox(nullptr, message, L"Runtime Error", 0);
				}

                // Model
                char manufacturerBuffer[vr::k_unMaxPropertyStringSize] = "<ERROR>";
                vr::VRSystem()->GetStringTrackedDeviceProperty(i, vr::Prop_ManufacturerName_String, manufacturerBuffer, vr::k_unMaxPropertyStringSize, &pError);
                if (pError != vr::TrackedProp_Success && pError != vr::TrackedProp_UnknownProperty) {
                    wchar_t message[1024];
                    swprintf(message, 1024, L"Could not get Prop_ManufacturerName of device %i: %s", i, vr::VRSystem()->GetPropErrorNameFromEnum(pError));
                    MessageBox(nullptr, message, L"Runtime Error", 0);
				}
                char modelNumberBuffer[vr::k_unMaxPropertyStringSize] = "<ERROR>";
                vr::VRSystem()->GetStringTrackedDeviceProperty(i, vr::Prop_ModelNumber_String, modelNumberBuffer, vr::k_unMaxPropertyStringSize, &pError);
                if (pError != vr::TrackedProp_Success && pError != vr::TrackedProp_UnknownProperty) {
                    wchar_t message[1024];
                    swprintf(message, 1024, L"Could not get Prop_ModelNumber of device %i: %s", i, vr::VRSystem()->GetPropErrorNameFromEnum(pError));
                    MessageBox(nullptr, message, L"Runtime Error", 0);
				}
                char trackingSystemBuffer[vr::k_unMaxPropertyStringSize] = "<ERROR>";
                vr::VRSystem()->GetStringTrackedDeviceProperty(i, vr::Prop_TrackingSystemName_String, trackingSystemBuffer, vr::k_unMaxPropertyStringSize, &pError);
                if (pError != vr::TrackedProp_Success && pError != vr::TrackedProp_UnknownProperty) {
                    wchar_t message[1024];
                    swprintf(message, 1024, L"Could not get Prop_TrackingSystemName of device %i: %s", i, vr::VRSystem()->GetPropErrorNameFromEnum(pError));
                    MessageBox(nullptr, message, L"Runtime Error", 0);
				}

                // Status
                vr::TrackedDevicePose_t poses[vr::k_unMaxTrackedDeviceCount];
                vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, i, poses, vr::k_unMaxTrackedDeviceCount);
                vr::TrackedDevicePose_t *pose = poses + i;


                // Now display
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); // ID
                ImGui::Text("%i", i);

                ImGui::TableSetColumnIndex(1); // Class
                ImGui::Text(deviceClassString);

                ImGui::TableSetColumnIndex(2); // Role (_waist, _foot, given by the driver)
                ImGui::Text(roleBuffer);

                ImGui::TableSetColumnIndex(3); // Battery %

                // Show battery only for HMD, Controller and Trackers
                if (deviceClass == 1 || deviceClass == 2 || deviceClass == 3) {
                    if (deviceBatteryPercent <= (cfgBatteryWarn / 100) && deviceBatteryPercent > (cfgBatteryLow / 100)) {
                        ImGui::PushStyleColor(ImGuiCol_Text, BAT_WARN);
                    } else if (deviceBatteryPercent <= (cfgBatteryLow / 100)) {
                        ImGui::PushStyleColor(ImGuiCol_Text, BAT_LOW);
                    } else {
                        ImGui::PushStyleColor(ImGuiCol_Text, BAT_OK);
                    }

                    if (deviceIsCharging) {
                        ImGui::Text("%.0f%% CHG", deviceBatteryPercent * 100);
                    } else {
                        ImGui::Text("%.0f%%", deviceBatteryPercent * 100);
                    }
                    ImGui::PopStyleColor();
                }

                ImGui::TableSetColumnIndex(4); // Model
                ImGui::Text("%s, %s\nTRK: %s", manufacturerBuffer, modelNumberBuffer, trackingSystemBuffer);

                ImGui::TableSetColumnIndex(5); // Serial
                ImGui::Text(serialBuffer);

                ImGui::TableSetColumnIndex(6); // Status (Running_OK, ...) Tracking Result
                switch (pose->eTrackingResult) {
                    case 1:
                        ImGui::PushStyleColor(ImGuiCol_Text, TRACKING_WAIT);
                        ImGui::Text("Uninitialized");
                        break;
                    case 100:
                        ImGui::PushStyleColor(ImGuiCol_Text, TRACKING_WAIT);
                        ImGui::Text("Calibrating_InProgress");
                        break;
                    
                    case 101:
                        ImGui::PushStyleColor(ImGuiCol_Text, TRACKING_WAIT);
                        ImGui::Text("Calibrating_OutOfRange");
                        break;
                    
                    case 200:
                        ImGui::PushStyleColor(ImGuiCol_Text, TRACKING_OK);
                        ImGui::Text("Running_OK");
                        break;
                    
                    case 201:
                        ImGui::PushStyleColor(ImGuiCol_Text, TRACKING_KO);
                        ImGui::Text("Running_OutOfRange");
                        break;
                }
                ImGui::PopStyleColor();
            }
        }

        ImGui::EndTable();
    }
}

void Show_Settings() {
    ImGui::Checkbox("Alert Low Battery", &cfgAlertLowBattery);

    ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x + 100);
    ImGui::SetNextItemWidth(300);
    ImGui::SliderFloat("Alert threshold", &cfgBatteryLow, 0, 100, nullptr);

    ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x + 100);
    ImGui::SetNextItemWidth(300);
    ImGui::SliderFloat("Warn threshold", &cfgBatteryWarn, 0, 100, nullptr);
}
