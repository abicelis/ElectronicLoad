



void handleModeSelection() {
  //Disable output, clear LCD, reset ISet=ISET_CC_DEFAULT, Encoder=0
  writeISet(0);
  myEnc.write(0);
  outputEnabled = false;
  disableTimer2();
  resetLcdToDefault();
  lcd.setCursor(0,0); 

  if(mode == CONSTANT_CURRENT){
    DEBUG_PRINTLN("CONSTANT_RESISTANCE");
    mode = CONSTANT_RESISTANCE;
    lcd.write("C.Res:");
    ISetVal = ISET_CR_DEFAULT;
  }
  else if(mode == CONSTANT_RESISTANCE) {
    DEBUG_PRINTLN("CONSTANT_POWER");
    mode = CONSTANT_POWER;
    lcd.write("C.Pow:");
    ISetVal = ISET_CP_DEFAULT;
  }
  else if(mode == CONSTANT_POWER) {
    DEBUG_PRINTLN("SQUARE_CURRENT");
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
    DEBUG_PRINTLN("TRIANGLE_CURRENT");
    mode = TRIANGLE_CURRENT;
    lcd.createChar(5, triang1);
    lcd.createChar(6, triang2);
    lcd.home();
    lcd.write(byte(5));
    lcd.write(byte(6));
    lcd.write(byte(5));
    lcd.write(":");
    TCisHi = false;
    ISetTimeOverflowVal = 0;
    ISetTimeCounter = 0; 
  }
  else if(mode == TRIANGLE_CURRENT) {
    DEBUG_PRINTLN("SINE_CURRENT");
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
    DEBUG_PRINTLN("CONSTANT_CURRENT");
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
