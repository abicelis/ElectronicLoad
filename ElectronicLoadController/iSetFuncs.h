void writeISet(int val){
  if(mode == CONSTANT_CURRENT || mode == SQUARE_CURRENT || mode == SINE_CURRENT || mode == TRIANGLE_CURRENT) {
    /*DEBUG_PRINT("writeISet function called! CONSTANT_CURRENT=");
    DEBUG_PRINT(ISetOneAmp);
    DEBUG_PRINT(" val=");
    DEBUG_PRINT(val);
    DEBUG_PRINT(" PWM=");
    DEBUG_PRINTLN((uint16_t)(((double)val/1000)*ISetOneAmp));*/
    analogWrite16(ISetPin, (uint16_t)(((double)val/1000)*ISetOneAmp));
  } else if (mode == CONSTANT_RESISTANCE) {
    /*DEBUG_PRINT("writeISet function called! CONSTANT_RESISTANCE=");
    DEBUG_PRINT(" val=");
    DEBUG_PRINT(val);
    DEBUG_PRINT(" PWM=");
    DEBUG_PRINTLN((uint16_t)(((double)VLoad/ISetVal)*ISetOneAmp));*/
    if(val == 0)
      analogWrite16(ISetPin, 0); //This one's needed cause when ISetVal==0 the below eq explodes, (ISetVal-1) division by -1?
    else
      analogWrite16(ISetPin, (uint16_t)(((double)VLoad/(val-1))*ISetOneAmp)); //-1 ohm sense resistors
  } else if (mode == CONSTANT_POWER) {
    /*DEBUG_PRINT("writeISet function called! CONSTANT_POWER=");
    DEBUG_PRINT(" val=");
    DEBUG_PRINT(val);
    DEBUG_PRINT(" PWM=");
    DEBUG_PRINTLN((uint16_t)(((double)VLoad/ISetVal)*ISetOneAmp));*/
    if(val == 0)
      analogWrite16(ISetPin, 0); //This one's needed cause when ISetVal==0 the below eq explodes, (ISetVal-1) division by -1?
    else if(VLoad == 0)
      analogWrite16(ISetPin, ISetOneAmp); //If val!=0 (Some power is wanted) and VLoad is zero, Prevent division by zero, and basically short the Load.
    else
      analogWrite16(ISetPin, (uint16_t)(  (((double)val/1000) / VLoad) * ISetOneAmp));
  }
}       

void calibrateISet() {
  
  //Starting calibration
  DEBUG_PRINTLN("Calibrating ILOAD_PIN:");
  lcd.clear();
  lcd.print("Starting ISet");
  lcd.setCursor(0, 1);
  lcd.print("Calibration..");
  delay(1000);                               


  //Set ARef to internal MCU's 1.1v
  analogReference(INTERNAL); 



  int ISetCalibrationVal = 0;                       //ISetCalibrationVal is 16bit. After calibration, ISetCalibrationVal should be set somewhere around 2v (Around 6k and 8k)
  for(int i = 6000; i < 8000; i=i+50) {     
    
    lcd.clear();
    lcd.print("Testing ");
    lcd.print(i);
    lcd.print("..");
    DEBUG_PRINT("Testing ISetPin=");
    DEBUG_PRINT(i);
    DEBUG_PRINTLN("...");
  
    analogWrite16(ISetPin, i);                                //Set a test value
    delay(100);                                               //Delay a bit for the ADC to settle                                  
    ISetCalibrationVal = analogRead(ISetCalibrationPin);      //Read a 10bit (0-1023) value where 1023 = 1.1v   
    float ISetVolts = (((float)ISetCalibrationVal/1024.0)*1.1);
   
    lcd.setCursor(0, 1);
    lcd.print("Got ");
    lcd.print(ISetVolts);    
    lcd.print("v");
    DEBUG_PRINT("Result: ISetCalibrationVal=");
    DEBUG_PRINT(ISetCalibrationVal);
    DEBUG_PRINT(" [");
    DEBUG_PRINT((((float)ISetCalibrationVal/1023.0)*1.1));    
    DEBUG_PRINT("v]");
  
    if(ISetVolts < 0.5) {                          //If ISetVolts < 0.5v, not good!                                         
       lcd.print(" [BAD]"); 
       DEBUG_PRINTLN(" [BAD]");
       delay(200);
    } else {
       lcd.print(" [GOOD]"); 
       DEBUG_PRINTLN(" [GOOD]");
       ISetOneAmp = 2*i;                          //Calculate ISetOneAmp
       delay(1000);
       break;
    }
  }

  //If we got here, it means we have a good ISetCalibrationVal!
  myEnc.write(ISetOneAmp);                         
  DEBUG_PRINTLN("Tweaking ISetOneAmp in LCD");

  bool tweakingISet = true;
  long oldISet = -999;
  bool textFlag = false;
  unsigned long currentMillis = millis() - 3001;
  while(tweakingISet) {
      
    long newISet = myEnc.read();
    
    if(newISet != oldISet) {
      oldISet = newISet;
      lcd.setCursor(0,0);
      lcd.print("ISet[1v]=       ");
      lcd.setCursor(9,0);
      lcd.print(oldISet);
      DEBUG_PRINT("ISetOneAmp tweaked to ");
      DEBUG_PRINTLN(oldISet);
      analogWrite16(ISetPin, oldISet);
    }
  
    //Change text every second
    if(millis() - currentMillis > 3000) {
      currentMillis = millis();
      textFlag = !textFlag;
      lcd.setCursor(0,1);
  
      if(textFlag)
        lcd.print("Trn Knob = Tweak");
      else
        lcd.print("Click Knob = OK ");
    }

    if(digitalRead(encBtn) == LOW)  { //button pressed
      ISetOneAmp = oldISet;
      DEBUG_PRINT("ISetOneAmp set to ");
      DEBUG_PRINTLN(ISetOneAmp);

      lcd.setCursor(0,1);
      lcd.print("ISet config DONE");
      tweakingISet = false;
      delay(2000);
    }  

  }
  
  //Clear LCD, set ISet to 0v, reset analogReference
  lcd.clear();
  ISetVal = ISET_CC_DEFAULT;
  myEnc.write(0);
  writeISet(ISetVal);
  analogReference(DEFAULT); 

}
