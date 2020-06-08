void handleOutputToggle() {
    outputEnabled = !outputEnabled;
    if(outputEnabled) {
      enableTimer2();
    } else { 
      disableTimer2();
      writeISet(0);
    }
}

void nextMode() {
   switch(mode) {
    case CONSTANT_CURRENT:
      mode = CONSTANT_RESISTANCE;
      break;
    case CONSTANT_RESISTANCE:
      mode = CONSTANT_POWER;
      break;
    case CONSTANT_POWER:
      mode = SQUARE_CURRENT;
      break;
    case SQUARE_CURRENT:
      mode = TRIANGLE_CURRENT;
      break;
    case TRIANGLE_CURRENT:
      mode = SINE_CURRENT;
      break;
    case SINE_CURRENT:
      mode = CONSTANT_CURRENT;
      break;
   }
}

void handleModeSelection(bool goToNextMode) {
  if(goToNextMode) {
    nextMode();
  }

  forceLCDRedraw = true;
  if(outputEnabled)
    handleOutputToggle();
  
  //Clear LCD, reset encoder.
  myEnc.write(0);
  resetLcdToDefault();
  lcd.setCursor(0,0); 

  if(mode == CONSTANT_CURRENT) {
    DEBUG_PRINTLN("CONSTANT_CURRENT");
    lcd.write("CC");
    ISetVal = ISET_CC_DEFAULT;
  }
  else if(mode == CONSTANT_RESISTANCE){
    DEBUG_PRINTLN("CONSTANT_RESISTANCE");
    lcd.write("CR");
    ISetVal = ISET_CR_DEFAULT;
  }
  else if(mode == CONSTANT_POWER) {
    DEBUG_PRINTLN("CONSTANT_POWER");
    lcd.write("CP");
    ISetVal = ISET_CP_DEFAULT;
  }
  else if(mode == SQUARE_CURRENT) {
    DEBUG_PRINTLN("SQUARE_CURRENT");
    lcd.createChar(5, squaree);
    lcd.home();                       //lcd needs this line otherwise it wont display created custom char
    lcd.write(byte(5));
    lcd.write(byte(5));
    ISetTimeOverflowVal = ISetSCTHi;
    ISetVal = ISetSCIHi;        
    ISetTimeCounter = 0;
  }
  else if(mode == TRIANGLE_CURRENT) {
    DEBUG_PRINTLN("TRIANGLE_CURRENT");
    lcd.createChar(5, triang1);
    lcd.createChar(6, triang2);
    lcd.home();
    lcd.write(byte(5));
    lcd.write(byte(6));
    TCisHi = false;
    ISetTimeOverflowVal = 0;
    ISetTimeCounter = 0; 
  }
  else if(mode == SINE_CURRENT) {
    DEBUG_PRINTLN("SINE_CURRENT");
    lcd.createChar(5, sine1);
    lcd.createChar(6, sine2);
    lcd.home();
    lcd.write(byte(5));
    lcd.write(byte(6));
    ISetVal = 0;
  }
}



void handleEncoderButtonPress() {
  forceLCDRedraw = true; //This pretty much forces the LCD to be re-drawn

  if(mode == SQUARE_CURRENT) {
    switch(SCCurrentParam) {
      case SC_PARAM_IHI:
        SCCurrentParam = SC_PARAM_ILO;  break;
      case SC_PARAM_ILO:
        SCCurrentParam = SC_PARAM_THI;  break;
      case SC_PARAM_THI:
        SCCurrentParam = SC_PARAM_TLO;  break;
      case SC_PARAM_TLO:
        SCCurrentParam = SC_PARAM_IHI;  break;
    }
  } else if(mode == TRIANGLE_CURRENT) {
    switch(TCCurrentParam) {
      case TC_PARAM_IHI:
        TCCurrentParam = TC_PARAM_ILO;  break;
      case TC_PARAM_ILO:
        TCCurrentParam = TC_PARAM_THI;  break;
      case TC_PARAM_THI:
        TCCurrentParam = TC_PARAM_TLO;  break;
      case TC_PARAM_TLO:
        TCCurrentParam = TC_PARAM_IHI;  break;
    }
  } else if(mode == SINE_CURRENT) {
    switch(SNCCurrentParam) {
      case SNC_PARAM_IHI:
        SNCCurrentParam = SNC_PARAM_ILO;  break;
      case SNC_PARAM_ILO:
        SNCCurrentParam = SNC_PARAM_T;  break;
      case SNC_PARAM_T:
        SNCCurrentParam = SNC_PARAM_IHI;  break;
    }      
  }
}


int getEncoderMovement() {
    int newPos = myEnc.read();

    int movement = 0;
    if(abs(newPos) >= 4) {
      movement =  0.5 + pow(newPos/4, 2); //pow returns a float and floath math is weird
      if(newPos < 0) movement *= -1; //pow removes sign, add it back
      myEnc.write(0);
    }
    return movement;
}
