int getLenghtOfFloat(float var)
{
  if(var >= 10)
    return 5;
  else
    return 4;
}
int getLenghtOfInt(int var)
{
  if(var >= 1000)
    return 4; 
  if(var >= 100)
    return 3;  
  else if(var >= 10)
    return 2;
  else
    return 1;
}


void resetLcdToDefault() {
  lcd.clear();
  lcd.setCursor(9, 0);
  lcd.write(byte(1)); //VL= char

  lcd.setCursor(15, 0);
  lcd.write("v");
  lcd.setCursor(9, 1);
  lcd.write(byte(2)); //IL= char
  lcd.setCursor(14, 1);
  lcd.write("mA");
}

void handleSCParamSelection() {
  if(SCCurrentParam == SC_PARAM_IHI) {
    SCCurrentParam = SC_PARAM_ILO;
  } else if(SCCurrentParam == SC_PARAM_ILO) {
    SCCurrentParam = SC_PARAM_THI;
  } else if(SCCurrentParam == SC_PARAM_THI) {
    SCCurrentParam = SC_PARAM_TLO;
  } else if(SCCurrentParam == SC_PARAM_TLO) {
    SCCurrentParam = SC_PARAM_IHI;
  }
}

void handleModeSelection() {
  //Disable output, clear LCD, reset ISet=ISET_CC_DEFAULT, Encoder=0
  writeISet(0);
  myEnc.write(0);
  outputEnabled = false;
  digitalWrite(outputEnabledLed, LOW);
  resetLcdToDefault();
  lcd.setCursor(0,0); 

  if(mode == CONSTANT_CURRENT){
    DEBUG_PRINT("CONSTANT_RESISTANCE");
    mode = CONSTANT_RESISTANCE;
    lcd.write("C.Res:");
    ISetVal = ISET_CR_DEFAULT;
  }
  else if(mode == CONSTANT_RESISTANCE) {
    DEBUG_PRINT("CONSTANT_POWER");
    mode = CONSTANT_POWER;
    lcd.write("C.Pow:");
    ISetVal = ISET_CP_DEFAULT;
  }
  else if(mode == CONSTANT_POWER) {
    DEBUG_PRINT("SQUARE_CURRENT");
    mode = SQUARE_CURRENT;
    lcd.createChar(5, squaree);
    lcd.home();                       //lcd needs this line otherwise it wont display created custom char
    lcd.write(byte(5));
    lcd.write(byte(5));
    lcd.write(byte(5));
    lcd.write(":");
    ISetTimeOverflowVal = ISetSCTHi;
    ISetVal = ISetSCIHi;        
    ISetTimeCounter = 0;
  }
  else if(mode == SQUARE_CURRENT) {
    DEBUG_PRINT("TRIANGLE_CURRENT");
    mode = TRIANGLE_CURRENT;
    lcd.createChar(5, triang1);
    lcd.createChar(6, triang2);
    lcd.home();
    lcd.write(byte(5));
    lcd.write(byte(6));
    lcd.write(byte(5));
    lcd.write(":");
    ISetVal = 0;
  }
  else if(mode == TRIANGLE_CURRENT) {
    DEBUG_PRINT("SINE_CURRENT");
    mode = SINE_CURRENT;
    lcd.createChar(5, sine1);
    lcd.createChar(6, sine2);
    lcd.home();
    lcd.write(byte(5));
    lcd.write(byte(6));
    lcd.write(byte(5));
    lcd.write(":");
    ISetVal = 0;
  }
  else if(mode == SINE_CURRENT) {
    DEBUG_PRINT("CONSTANT_CURRENT");
    mode = CONSTANT_CURRENT;
    lcd.write("C.Cur:");
    ISetVal = ISET_CC_DEFAULT;
  }

}

int getEncoderMovement() {
    int newPos = myEnc.read();
    if(newPos >= 4) {
      myEnc.write(0);
      return 1;
    } else if(newPos <= -4) {
      myEnc.write(0);
      return -1;
    }
    return 0;
}

/*
 *@Param numberChars: The "lenght" of the int, number 999 has 3 numberChars
 */
void lcdPrintIntWhitespace(int val, byte numberChars) {
  for(int i = 0; i < numberChars-getLenghtOfInt(val) ; i++) {
    lcd.print(" ");
  }
}

