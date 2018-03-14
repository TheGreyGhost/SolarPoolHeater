#include "Settings.h"
#include <EEPROM.h>
#include <stdlib.h>

const float defaults[] = {9.0, 19.0, 28.0, 1.0, 
					100.0, 1.0, 180.0,
					5.0, 60.0, 45.0, 4.0, 0.0};

const char* settingLabels[NUMBER_OF_EEPROM_SETTINGS] = {"onTimeHours", "offTimeHours", "temperatureSetpoint", "temperatureSetpointHysteresis",
										 "solarIntensityThreshold", "minimumHotInletMinusColdInlet", "belowMinimumTimeoutSeconds",
										 "maxDailySystemErrorCount", "hotInletAlarm", "coldOutletAlarm", "minSecondsPerFourPumpOns", "INVALID_PARAMETER"};

float setSetting(EEPromSettings whichSetting, float newValue)
{
  EEPROM.put(getEEPROMaddress(whichSetting), newValue);
  return EEPROM.get(getEEPROMaddress(whichSetting));	
}

float getSetting(EEPromSettings whichSetting)
{
  return EEPROM.get(getEEPROMaddress(whichSetting));	
}

float setSettingDefault(EEPromSettings whichSetting)
{
  int defaultidx = (int)clipSetting(whichSetting);
  return setSetting(whichSetting, defaults[defaultidx]);
}

void setSettingDefaultAll(EEPromSettings whichSetting)
{
  EEPromSettings setting;
  for (setting = SET_FIRST; setting <= SET_INVALID; ++setting) {
    setSettingDefault(setting);
  }
}

// clips the setting to the valid range.  Forces to SET_INVALID if necessary.
EEPromSettings clipSetting(EEPromSettings whichSetting)
{
  if (whichSetting >= SET_FIRST && whichSetting < SET_INVALID) {
    return whichSetting;
  } else {
    return SET_INVALID;
  }
}

// gets the EEPROM address for the given setting
int getEEPROMaddress(EEPromSettings whichSetting)
{
  return 4 * (int)clipSetting(whichSetting);
}


// returns a text description of the indicated setting
const char *getEEPROMSettingLabel(EEPromSettings whichSetting)
{
  return settingLabels[getEEPROMaddress(whichSetting)];
}
