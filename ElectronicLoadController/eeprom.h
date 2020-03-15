int eepromVarsWrittenAddress = 100;
int eepromVarsWrittenCheck = 193;

struct EepromVars {
  float VLoadTweakVar;
  int   ILoadTweakVar;
  //TODO uint16_tisetoneamp
};

void saveEepromValues() {
  DEBUG_PRINTLN("Saving EEPROM vars");

  EepromVars vars;
  vars.VLoadTweakVar = VLoadTweak;
  vars.ILoadTweakVar = ILoadTweak;
  EEPROM.put(0, vars);
  DEBUG_PRINTLN("DONE!");

  if(EEPROM.get(eepromVarsWrittenAddress, eepromVarsWrittenCheck) != eepromVarsWrittenCheck) {
    DEBUG_PRINTLN("Saved them for the first time!");
    EEPROM.put(eepromVarsWrittenAddress, eepromVarsWrittenCheck);
  }
}

void loadEepromValues() {
  DEBUG_PRINTLN("Load EEPROM vars");
  
  int check = 0;
  EEPROM.get(eepromVarsWrittenAddress, check);
  if(check == eepromVarsWrittenCheck) {
    //Values have been put, retreive them.

    EepromVars vars; // Variable to store custom object read from EEPROM.
    EEPROM.get(0, vars);

    DEBUG_PRINTLN("Values exist, loading them.");
    DEBUG_PRINT("VLoadTweakVar=");
    DEBUG_PRINTLN(vars.VLoadTweakVar);
    DEBUG_PRINT("ILoadTweakVar=");
    DEBUG_PRINTLN(vars.ILoadTweakVar);

    VLoadTweak = vars.VLoadTweakVar;
    ILoadTweak = vars.ILoadTweakVar;
  } else {
    DEBUG_PRINTLN("Values DONT exist in EEPROM.");
  }
}

void clearEeprom() {
  DEBUG_PRINTLN("Zeroing EEPROM");
  for (int index = 0 ; index < 50 ; index++) {
    EEPROM[ index ] = 0;
  }
  DEBUG_PRINTLN("Zeroing EEPROM DONE");
}
