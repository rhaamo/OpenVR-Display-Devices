#pragma once

#include "config.h"
#include "configuration.h"
#include "notifications.h"

#include <openvr.h>

#include <array>
#include <format>

// Should be plenty sufficient...
#define MAX_INDEXES 20
// Every five minutes
#define CHECK_EVERY 5 * 60

class BatteryChecker {
private:
	std::array<int, MAX_INDEXES> gauges;
	std::array<std::string, MAX_INDEXES> gaugesNames;
	std::array<bool, MAX_INDEXES> gaugesCharging;
	time_t lastCheck;

	bool validIndex(int index) {
		if (gauges.empty()) {
			return false;
		}
		if (index > MAX_INDEXES) {
			return false;
		}
		if (index > gauges.size()) {
			return false;
		}
		return true;
	}
	void dispatchLowNotification(int index) {
		std::string title = "Low tracker battery !";
		std::string content = std::format("Tracker {} is low on battery: {}%", gaugesNames[index], gauges[index]).c_str();

		std::cout << "LOW battery for tracker '" << gaugesNames[index] << "' id " << index << " level: " << gauges[index] << std::endl;
		sendNotification(title, content);
	}
	void dispatchWarnNotification(int index) {
		std::string title = "Tracker battery warning !";
		std::string content = std::format("Tracker {} battery warning: {}%", gaugesNames[index], gauges[index]).c_str();

		std::cout << "WARN battery for tracker '" << gaugesNames[index] << "' id " << index << " level: " << gauges[index] << std::endl;
		sendNotification(title, content);
	}
	void dispatchSlowChargeNotification(int index) {
		std::string title = "Slow charge tracker battery !";
		std::string content = std::format("Tracker {} charging too slowly ! {}%", gaugesNames[index], gauges[index]).c_str();

		std::cout << "WARN charging too slow for tracker '" << gaugesNames[index] << "' id " << index << " level: " << gauges[index] << std::endl;
		sendNotification(title, content);
	}

public:
	BatteryChecker() {
		gauges.fill(-99);
		gaugesNames.fill("<unknown>");
		gaugesCharging.fill(false);
		lastCheck = time(0);
	}
	~BatteryChecker() {
		delete &gauges;
	}
	void updateGauge(int index, int value, std::string role, bool charging) {
		gauges[index] = value;
		gaugesNames[index] = role;
		gaugesCharging[index] = charging;
	}
	bool isWarn(int index) {
		if (!validIndex(index)) {
			return false;
		}

		return (gauges[index] > application_configuration.batteryLow && gauges[index] <= application_configuration.batteryWarn);
	}
	bool isLow(int index) {
		if (!validIndex(index)) {
			return false;
		}

		return (gauges[index] <= application_configuration.batteryLow);
	}
	void checkGaugeAndDispatchNotifications(int index) {
		// First check we have a valid index
		if (!validIndex(index)) {
			std::cout << "[checkGaugeAndDispatchNotifications] We have an invalid index lol" << std::endl;
			return;
		}

		// TODO slow charging: if charging && new% < old%

		// Do a check only if lastCheck interval matches
		if (difftime(time(0), lastCheck) >= CHECK_EVERY) {
			if (isLow(index)) {
				dispatchLowNotification(index);
			} else if (isWarn(index)) {
				dispatchWarnNotification(index);
			}
			std::cout << "[checkGaugeAndDispatchNotifications] Still ok i guess" << std::endl;
			// That's it, set check time to current time
			lastCheck = time(0);
		}
		// Do nothing as we still haven't waited enough
	}
};

