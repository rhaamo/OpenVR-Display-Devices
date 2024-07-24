#pragma once

// lol
#define UNICODE

#include "config.h"

#include <SimpleIni.h>
#include <platform_folders.h>
#include <string>
#include <filesystem>
#include <iostream>

struct AppConfig {
    // Internal use
    bool loaded = false;

    // Config section battery_alert
    bool alertLowBattery = false;
    float batteryWarn = 50;
    float batteryLow = 20;
};

extern AppConfig application_configuration;

std::string getConfigFolder();
bool testAndCreatePathToConfigFile(std::string configPath);
void loadOrInitConfiguration(AppConfig &cfg);
void saveConfiguration(AppConfig &cfg);
