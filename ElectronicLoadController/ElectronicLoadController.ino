#define DEBUG

#include <LiquidCrystal.h>
#include <Encoder.h>
#include "variables.h"
#include "pwm16Bit.h"
#include "iSetFuncs.h"
#include "miscFuncs.h"
#include "vSensorFuncs.h"
#include "variableCurrentTimersAndISRs.h"


void setup() {

  #ifdef DEBUG
    Serial.begin(115200);
  #endif

  //ILOAD_PIN as PWM output
  setupPWM16();

  //Configure TIMER2 so that it calls ISR(TIMER2_COMPA_vect) @ 500Hz (every 2mS)
  //ISR(TIMER2_COMPA_vect) is used to update CC waveforms
  setupTimer2();

  //LCD Setup
  lcd.begin(16, 2);
  lcd.print("ElectronicLoad");
  lcd.setCursor(0,1);
  lcd.print("  [Version 1.07]");
  lcd.createChar(0, ohm);
  lcd.createChar(1, vLoad);
  lcd.createChar(2, iLoad);
  lcd.createChar(3, hi);
  lcd.createChar(4, lo);

  delay(1500);

  //Button and Led pinmode setup
  pinMode(OUTPUT, outputEnabledLed);
  pinMode(INPUT, encBtn);
  digitalWrite(encBtn, HIGH);                   //Enable internal pullup
  pinMode(INPUT, modeSelectorBtn);
  digitalWrite(modeSelectorBtn, HIGH);          //Enable internal pullup
  pinMode(INPUT, outputEnableBtn);
  digitalWrite(outputEnableBtn, HIGH);          //Enable internal pullup

  //ISet calibration (Finds a proper ISetOneAmp value where IL=1A)
  //calibrateISet();
  ISetOneAmp=14280;

  resetLcdToDefault();
  lcd.setCursor(0, 0);
  lcd.write("C.Cur:");

}


void loop() {

 if(mode == CONSTANT_CURRENT) {
    int encoderPos = getEncoderMovement();       //getEncoderMovement() returns either -1, 0 or 1
    if(encoderPos > 0 && ISetVal < ISET_CC_MAX)   //Encoder incremented
      ISetVal+=ISET_CC_STEP;
    else if (encoderPos < 0 && ISetVal >= ISET_CC_STEP)   //Encoder decremented
      ISetVal-=ISET_CC_STEP;

    lcd.setCursor(0, 1);
    lcdPrintIntWhitespace(ISetVal, 4);
    lcd.print(ISetVal); 
    lcd.write("mA");
  }

  
 if(mode == CONSTANT_RESISTANCE) {
    byte encoderPos = getEncoderMovement();       //getEncoderMovement() returns either -1, 0 or 1
    if(encoderPos > 0 && ISetVal < ISET_CR_MAX)   //Encoder incremented
      ISetVal+=ISET_CR_STEP;
    else if (encoderPos < 0 && ISetVal >= ISET_CR_MIN)   //Encoder decremented
      ISetVal-=ISET_CR_STEP;
   
    lcd.setCursor(0, 1);
    lcdPrintIntWhitespace(ISetVal, 4);
    lcd.print(ISetVal); 
    lcd.write(byte(0)); //ohm char
  }

    
 if(mode == CONSTANT_POWER) {
    byte encoderPos = getEncoderMovement();       //getEncoderMovement() returns either -1, 0 or 1
    if(encoderPos > 0 && ISetVal < ISET_CP_MAX)   //Encoder incremented
      ISetVal+=ISET_CP_STEP;
    else if (encoderPos < 0 && ISetVal >= ISET_CP_STEP)   //Encoder decremented
      ISetVal-=ISET_CP_STEP;
   
    lcd.setCursor(0, 1);
    lcdPrintIntWhitespace(ISetVal, 4);
    lcd.print(ISetVal); 
    lcd.write("mW");
  }

   if(mode == SQUARE_CURRENT) {
    int encoderPos = getEncoderMovement();     //encoderPos is either -1, 0 or 1
    if(encoderPos != 0) {                        //Encoder changed
      switch(SCCurrentParam) {
      case SC_PARAM_IHI:
        if(encoderPos > 0 && ISetSCIHi < ISET_SC_IMAX)  
          ISetSCIHi += ISET_SC_I_STEP;
        else if(encoderPos < 0 && ISetSCIHi >= ISET_SC_I_STEP)           
          ISetSCIHi -= ISET_SC_I_STEP;   
        break;
      case SC_PARAM_ILO:
        if(encoderPos > 0 && ISetSCILo < ISET_SC_IMAX)  
          ISetSCILo += ISET_SC_I_STEP;
        else if(encoderPos < 0 && ISetSCILo >= ISET_SC_I_STEP)           
          ISetSCILo -= ISET_SC_I_STEP;   
        break;
      case SC_PARAM_THI:
        if(encoderPos > 0 && ISetSCTHi < ISET_SC_TMAX)  
          ISetSCTHi += ISET_SC_T_STEP;
        else if(encoderPos < 0 && ISetSCTHi >= ISET_SC_T_STEP)           
          ISetSCTHi -= ISET_SC_T_STEP;   
        break;
      case SC_PARAM_TLO:
      if(encoderPos > 0 && ISetSCTLo < ISET_SC_TMAX)  
          ISetSCTLo += ISET_SC_T_STEP;
        else if(encoderPos < 0 && ISetSCTLo >= ISET_SC_T_STEP)           
          ISetSCTLo -= ISET_SC_T_STEP;  
        break;
      }
    }

    lcd.setCursor(0, 1);
    switch(SCCurrentParam) {
      case SC_PARAM_IHI:
        lcd.write("I");
        lcd.write(byte(3));
        lcdPrintIntWhitespace(ISetSCIHi, 4);
        lcd.print(ISetSCIHi);
        lcd.write("mA");
        break;
        
      case SC_PARAM_ILO:
        lcd.write("I");
        lcd.write(byte(4));
        lcdPrintIntWhitespace(ISetSCILo, 4);
        lcd.print(ISetSCILo);
        lcd.write("mA");
        break;
        
      case SC_PARAM_THI:
        lcd.write("T");
        lcd.write(byte(3));
        lcdPrintIntWhitespace(ISetSCTHi, 4);
        lcd.print(ISetSCTHi);
        lcd.write("mS");   
        break;
           
       case SC_PARAM_TLO:
        lcd.write("T");
        lcd.write(byte(4));
        lcdPrintIntWhitespace(ISetSCTLo, 4);
        lcd.print(ISetSCTLo);
        lcd.write("mS");
        break;
    }
  }

   if(mode == TRIANGLE_CURRENT) {
  

    
  }

   if(mode == SINE_CURRENT) {
  

    
  }



  //Always update and print VLoad and ILoad
  updateVLoad(); 
  updateILoad();
  printVLoadAndILoad();



  //Handle Mode change button press
  if(digitalRead(modeSelectorBtn) == LOW)  {
    handleModeSelection();

    //Prevent double triggering?
    delay(400);
  }


  //Handle Output Enable/Disable button press
  if(digitalRead(outputEnableBtn) == LOW)  {
    
    outputEnabled = !outputEnabled;
    if(outputEnabled) {
      digitalWrite(outputEnabledLed, HIGH);
      enableTimer2();
    } else { 
      digitalWrite(outputEnabledLed, LOW);
      disableTimer2();
      writeISet(0);
    }
    
    //Prevent double triggering?
    delay(400);
  }


  //Handle Encoder button press (toggle parameter change)
  if(digitalRead(encBtn) == LOW)  {
      if(mode == SQUARE_CURRENT) {
          handleSCParamSelection();
      } else if(mode == TRIANGLE_CURRENT) {
      
      } else if(mode == SINE_CURRENT) {
      
      }

    //Prevent double triggering?
    delay(200);
  } 


//A little delay for good luck
delay(10);

}




