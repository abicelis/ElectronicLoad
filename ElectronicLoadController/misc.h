/* VLOAD */
float getVLoadSample() {
  //VloadPin*K    K=(R1+R2)/R2=4.3742,  R1=9917ohm, R2=2939ohm.
  //Vin *---[R1]---* VLoadPin(Arduino) *---[R2]---* GND
  int pinRead = analogRead(VLoadPin);
  
  float sample = (((float)pinRead/1023)*5)*4.3742;
//  DEBUG_PRINT("VLoad pin=");
//  DEBUG_PRINT(pinRead);
//  DEBUG_PRINT("   voltage=");
//  DEBUG_PRINTLN(sample);
  
  return sample;                                                                   
}

void sampleVLoad() {
  VLoadBuffer -= VLoad;               //Remove the VLoad avg from buffer
  VLoadBuffer += getVLoadSample();    //Add a new sample
  VLoad = VLoadBuffer/BUFFER_SIZE;    //Recalculate VLoad avg
}



/* ILOAD */
int getILoadSample() { 
  int pinRead = analogRead(ILoadPin);
  int sample = ((float)pinRead/1023)*5*1000;  //ILoad in mA

//  DEBUG_PRINT("ILoad pin=");
//  DEBUG_PRINT(pinRead);
//  DEBUG_PRINT("   mA=");
//  DEBUG_PRINTLN(sample);
  return sample;
}

void sampleILoad() {
  ILoadBuffer -= ILoad;               //Remove the ILoad avg from buffer
  ILoadBuffer += getILoadSample();    //Add a new sample
  ILoad = ILoadBuffer/BUFFER_SIZE;    //Recalculate ILoad avg
}



/* LCD print things */


void printVLoadAndILoad() {
  float tweakedVLoad = VLoad + VLoadTweak;
  lcd.setCursor(10, 0);
  if(getLenghtOfFloat(tweakedVLoad)== 4)
    lcd.print(" ");
  lcd.print(tweakedVLoad, 2);

  int tweakedILoad = ILoad + ILoadTweak;
  lcd.setCursor(10, 1);
  lcdPrintIntWhitespace(tweakedILoad, 4);
  lcd.print(tweakedILoad); 
}

void printTemp() {
  int tempInC = ((float)currentTemperature/1023)*5*100;  
  lcd.setCursor(4, 0);
  lcdPrintIntWhitespace(tempInC, 3);
  lcd.print(tempInC); 
  lcd.write(byte(7));
}


/* Temperature (LM35) */
void sampleTempAndUpdateFan() {
  currentTemperature = analogRead(temperatureSensorPin);

  DEBUG_PRINTLN("Temp=");
  DEBUG_PRINTLN(currentTemperature);

  if(currentTemperature >= maxValidTempVal)
    currentTemperature = maxValidTempVal;
  
  analogWrite16(fanPin, map(currentTemperature, 0, maxValidTempVal, 0, 65535));
}

void checkForOverTemp() {
  if(currentTemperature > overheatTempVal && outputEnabled) {
      handleOutputToggle();
      lcd.clear();
      printTemp();

      lcd.setCursor(3,1);
      lcd.print("!OVERHEAT!");
      delay(OVERTEMP_DELAY_MS);
      lcd.clear();

      handleModeSelection(false);
  }
}
