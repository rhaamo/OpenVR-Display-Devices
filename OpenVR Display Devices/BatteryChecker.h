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
// Every 10s
#define CHECK_CHG_EVERY 10

class BatteryChecker {
private:
	std::array<int, MAX_INDEXES> gauges;
	std::array<int, MAX_INDEXES> gaugesPrev;
	std::array<std::string, MAX_INDEXES> gaugesNames;
	std::array<bool, MAX_INDEXES> gaugesCharging;
	std::array<bool, MAX_INDEXES> gaugesChargingPrev;
	std::array<time_t, MAX_INDEXES> gaugesChargingChangeTime;
	time_t lastCheck;

	bool validIndex(int index) {
		if (gauges.empty() || gaugesPrev.empty()) {
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
	void dispatchChargeWarn(int index) {
		std::string title, content;
		if (gaugesCharging[index]) {
			title = "HMD is now charging";
			content = std::format("HMD is now charging");
		} else {
			title = "HMD not charging !";
			content = std::format("WOOP WOOP PLS CHECK CABLE");
		}

		std::cout << "HMD Charge changed from " << gaugesChargingPrev[index] << " to " << gaugesCharging[index] << std::endl;
		sendNotification(title, content);
	}

public:
	BatteryChecker() {
		gauges.fill(-99);
		gaugesPrev.fill(-99);
		gaugesNames.fill("<unknown>");
		gaugesCharging.fill(false);
		gaugesChargingPrev.fill(false);
		gaugesChargingChangeTime.fill(time(0));
		lastCheck = time(0);
	}
	~BatteryChecker() {
		delete &gauges;
	}
	void updateGauge(int index, int value, std::string role, bool charging) {
		// Set previous value with current value
		gaugesPrev[index] = gauges[index];
		// Set current value
		gauges[index] = value;
		// Set name and charging
		gaugesNames[index] = role;

		if (gaugesCharging[index] != charging) {
			// If the last charging state is different that new one, update the charging change time index
			gaugesChargingChangeTime[index] = time(0);
			// And set previous value
			gaugesChargingPrev[index] = gaugesCharging[index];
		}
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
	void checkGaugeAndDispatchNotifications(int index, bool isHmd) {
		// First check we have a valid index
		if (!validIndex(index)) {
			std::cout << "[checkGaugeAndDispatchNotifications] We have an invalid index lol" << std::endl;
			return;
		}

		// Do a check only if lastCheck interval matches
		if (difftime(time(0), lastCheck) >= CHECK_EVERY) {
			if (gaugesCharging[index] && (gauges[index] < gaugesPrev[index]) && (gaugesPrev[index] != -99)) {
				// if charging && val < prevVal && prevVal != -99, then we are discharging while charging
				dispatchSlowChargeNotification(index);
			} else {
				// Then we are not in a discharging condition
				if (isLow(index)) {
					dispatchLowNotification(index);
				} else if (isWarn(index)) {
					dispatchWarnNotification(index);
				}
			}
			// That's it, set check time to current time
			lastCheck = time(0);
		}

		// Every X minutes, check for the state change
		std::cout << "prev= " << gaugesChargingPrev[index] << " new: " << gaugesCharging[index] << std::endl;

		if (isHmd && (gaugesChargingPrev[index] != gaugesCharging[index]) && (difftime(time(0), gaugesChargingChangeTime[index]) >= CHECK_CHG_EVERY)) {
			dispatchChargeWarn(index);
			// Reset time
			gaugesChargingChangeTime[index] = time(0);
		}

		// Do nothing as we still haven't waited enough
	}
};

