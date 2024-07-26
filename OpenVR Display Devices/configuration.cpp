#include "configuration.h"

CSimpleIniA cfgIni;

std::string getConfigFolder() {
	std::filesystem::path dirRoot = sago::getConfigHome();
	std::filesystem::path dirApp = APPLICATION_CONFIG_DIR;
	std::filesystem::path fullPath = dirRoot / dirApp;
	return fullPath.string();
}

bool testAndCreatePathToConfigFile(std::string configPath) {
	// Does the file eixsts ?
	if (!std::filesystem::exists(configPath)) {
		// Create the folders
		if (!std::filesystem::create_directory(configPath)) {
			throw std::runtime_error("Cannot create path to configuration file: " + configPath);
		}
	} else {
		return true;
	}
	return true;
}

void loadOrInitConfiguration(AppConfig &app_cfg) {
	std::string configPath = getConfigFolder();
	cfgIni.SetUnicode();

	if (testAndCreatePathToConfigFile(configPath)) {
		std::filesystem::path configFile = configPath / std::filesystem::path("app.ini");
		if (!std::filesystem::exists(configFile)) {
			// The config doesn't exists, so create it
			saveConfiguration(app_cfg);
		}
		// Then Read config
		SI_Error rc = cfgIni.LoadFile(configFile.c_str());
		if (rc < 0) {
			throw std::runtime_error("Cannot read or parse configuration file");
		}

		// Then set the struct from config
		app_cfg.alertLowBattery = cfgIni.GetBoolValue("battery_alert", "alert_low_on_battery", false, nullptr);
		app_cfg.batteryWarn = cfgIni.GetDoubleValue("battery_alert", "battery_warn", 50, nullptr);
		app_cfg.batteryLow = cfgIni.GetDoubleValue("battery_alert", "battery_low", 20, nullptr);
		app_cfg.notificationsWindows = cfgIni.GetBoolValue("notifications", "notifications_windows", false, nullptr);
		app_cfg.notificationsXsOverlay = cfgIni.GetBoolValue("notifications", "notifications_xsoverlay", false, nullptr);

		// Then set the config to the loaded state
		app_cfg.loaded = true;
	} else {
		throw std::runtime_error("Cannot create path to configuration file");
	}
}

void saveConfiguration(AppConfig &cfg) {
	std::string configPath = getConfigFolder();

	if (testAndCreatePathToConfigFile(configPath)) {
		std::filesystem::path configFile = configPath / std::filesystem::path("app.ini");

		if (cfgIni.SetBoolValue("battery_alert", "alert_low_on_battery", cfg.alertLowBattery) < 0) {
			throw std::runtime_error("Cannot save alertLowBattery to config");
		}
		if (cfgIni.SetDoubleValue("battery_alert", "battery_warn", cfg.batteryWarn) < 0) {
			throw std::runtime_error("Cannot save batteryWarn to config");
		}
		if (cfgIni.SetDoubleValue("battery_alert", "battery_low", cfg.batteryLow) < 0) {
			throw std::runtime_error("Cannot save batteryLow to config");
		}
		if (cfgIni.SetBoolValue("notifications", "notifications_windows", cfg.notificationsWindows) < 0) {
			throw std::runtime_error("Cannot save notificationsWindows to config");
		}
		if (cfgIni.SetBoolValue("notifications", "notifications_xsoverlay", cfg.notificationsXsOverlay) < 0) {
			throw std::runtime_error("Cannot save notificationsXsOverlay to config");
		}

		SI_Error rc = cfgIni.SaveFile(configFile.c_str());
		if (rc < 0) {
			throw std::runtime_error("Cannot save configuration file");
		}
	} else {
		throw std::runtime_error("Cannot create path to configuration file");
	}
}
