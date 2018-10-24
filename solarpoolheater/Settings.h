#ifndef SETTINGS_H
#define SETTINGS_H
#include "GlobalDefines.h"

enum EEPROMSettings {SET_FIRST = 0, SET_onTimeHours = 0, SET_offTimeHours = 1, SET_temperatureSetpoint = 2, SET_temperatureSetpointHysteresis = 3,
			SET_solarIntensityThreshold = 4, SET_minimumHotInletMinusColdInlet = 5, SET_belowMinimumTimeoutSeconds = 6, 
			SET_maxDailySystemErrorCount = 7, SET_hotInletAlarm = 8, SET_coldOutletAlarm = 9, SET_minSecondsPerFourPumpOns = 10, SET_dontRunPump = 11, 
			SET_logIntervalSeconds = 12, SET_logIntervalIdleSeconds = 13, SET_INVALID = 14};
const int NUMBER_OF_EEPROM_SETTINGS = (int)SET_INVALID + 1;

// change the given setting to a new value. if whichSetting is out of range, uses SET_INVALID.
// returns: the new value.
float setSetting(EEPROMSettings whichSetting, float newValue);

// changes the given setting to its default value. if whichSetting is out of range, uses SET_INVALID.
// returns: the new value.
float setSettingDefault(EEPROMSettings whichSetting);

// change all settings back to their default value
void setSettingDefaultAll();

// gets the given setting. if whichSetting is out of range, uses SET_INVALID.
float getSetting(EEPROMSettings whichSetting);

// returns a human-readable label for the given setting. if whichSetting is out of range, uses SET_INVALID.
const char *getEEPROMSettingLabel(EEPROMSettings whichSetting);

#endif
