#ifndef SETTINGS_H
#define SETTINGS_H

enum EEPROMSettings {SET_FIRST = 0, SET_onTimeHours = 0, SET_offTimeHours = 1, SET_temperatureSetpoint = 2, SET_temperatureSetpointHysteresis = 3,
			SET_solarIntensityThreshold = 4, SET_minimumHotInletMinusColdInlet = 5, SET_belowMinimumTimeoutSeconds = 6, 
			SET_maxDailySystemErrorCount = 7, SET_hotInletAlarm = 8, SET_coldOutletAlarm = 9, SET_minSecondsPerFourPumpOns = 10, SET_INVALID = 11};
const NUMBER_OF_EEPROM_SETTINGS = (int)SET_invalid + 1;

// change the given setting to a new value. 
// returns: the new value.
float setSetting(EEPromSettings whichSetting, float newValue);

// changes the given setting to its default value
// returns: the new value.
float setSettingDefault(EEPromSettings whichSetting);

// change all settings back to their default value
void setSettingDefaultAll();

// gets the given setting
float getSetting(EEPromSettings whichSetting);

// returns a human-readable label for the given setting
const char *getEEPROMSettingLabel(EEPromSettings whichSetting);

#endif
