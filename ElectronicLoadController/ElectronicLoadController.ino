#define DEBUG

#include <LiquidCrystal.h>
#include <EEPROM.h>


// This optional setting causes Encoder to use more optimized code,
// It must be defined before Encoder.h is included.
#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>
#include "values.h"
#include "lcd.h"
#include "pwm16Bit.h"
#include "iSet.h"
#include "voltageAndCurrentSensors.h"
#include "timer2.h"
#include "misc.h"
#include "eeprom.h"

unsigned long modeSelectorBtnLastPressMs = 0;
unsigned long outputEnableBtnLastPressMs = 0;
unsigned long encoderBtnLastPressMs = 0;
unsigned long LastVLoadILoadUpdateMs = 0;
unsigned long LastVLoadILoadSampleMs = 0;


void setup() {

  #ifdef DEBUG
    Serial.begin(115200);
  #endif
  
  //Set up ILOAD_PIN as 16-bit PWM output using Timer1
  setupILoadPWM16bit();

  //Configure TIMER2 so that it calls ISR(TIMER2_COMPA_vect) @ 500Hz (every 2mS)
  //ISR(TIMER2_COMPA_vect) is used to update CC waveforms
  setupTimer2();

  //LCD Setup
  lcd.begin(16, 2);
  lcd.print("ElectronicLoad");
  lcd.setCursor(0,1);
  lcd.print(" [Version 1.1.1]");
  lcd.createChar(0, ohm);
  lcd.createChar(1, vLoad);
  lcd.createChar(2, iLoad);
  lcd.createChar(3, hi);
  lcd.createChar(4, lo);

  delay(1500);

  //Button and Led pinmode setup
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


  //disable eeprom loading quickly
  //EEPROM.put(eepromVarsWrittenAddress, 123);

  //Load EEPROM values
  loadEepromValues();
}


void loop() {
  unsigned long currentMillis = millis();
  int encoderPos = getEncoderMovement();       //getEncoderMovement() returns either a negative value, 0 or a positive value
  
  if(mode == CONSTANT_CURRENT) {
    if(encoderPos < 0 && ISetVal >= ISET_CC_STEP) {
        ISetVal+=ISET_CC_STEP*encoderPos;
        if(ISetVal > ISET_CC_MAX) ISetVal = 0; //ISetVal is unsigned, if pushed lower than zero it'll loop back to Int.MAX!
          
    } else if(encoderPos > 0 && ISetVal < ISET_CC_MAX) {
        ISetVal+=ISET_CC_STEP*encoderPos;
        if(ISetVal > ISET_CC_MAX) ISetVal = ISET_CC_MAX;
    }
    
    lcd.setCursor(0, 1);
    lcdPrintIntWhitespace(ISetVal, 4);
    lcd.print(ISetVal); 
    lcd.write("mA");
  }


  else if(mode == CONSTANT_RESISTANCE) {
    if(encoderPos < 0 && ISetVal >= ISET_CR_STEP) {
        ISetVal+=ISET_CR_STEP*encoderPos;
        if(ISetVal > ISET_CR_MAX) ISetVal = 0; //ISetVal is unsigned, if pushed lower than zero it'll loop back to Int.MAX!
          
    } else if(encoderPos > 0 && ISetVal < ISET_CR_MAX) {
        ISetVal+=ISET_CR_STEP*encoderPos;
        if(ISetVal > ISET_CR_MAX) ISetVal = ISET_CR_MAX;
    }

    lcd.setCursor(0, 1);
    lcdPrintIntWhitespace(ISetVal, 4);
    lcd.print(ISetVal); 
    lcd.write(byte(0)); //ohm char
  }


  else if(mode == CONSTANT_POWER) {
    if(encoderPos < 0 && ISetVal >= ISET_CP_STEP) {
        ISetVal+=ISET_CP_STEP*encoderPos;
        if(ISetVal > ISET_CP_MAX) ISetVal = 0; //ISetVal is unsigned, if pushed lower than zero it'll loop back to Int.MAX!
          
    } else if(encoderPos > 0 && ISetVal < ISET_CP_MAX) {
        ISetVal+=ISET_CP_STEP*encoderPos;
        if(ISetVal > ISET_CP_MAX) ISetVal = ISET_CP_MAX;
    }
    
    lcd.setCursor(0, 1);
    lcdPrintIntWhitespace(ISetVal, 4);
    lcd.print(ISetVal); 
    lcd.write("mW");
  }

  else if(mode == SQUARE_CURRENT) {
    int encoderPos = getEncoderMovement();       //encoderPos is either -1, 0 or 1
    if(encoderPos != 0) {                        //Encoder changed
      switch(SCCurrentParam) {
      case SC_PARAM_IHI:
        if(encoderPos > 0 && ISetSCIHi < ISET_SC_IMAX)  
          ISetSCIHi += ISET_SC_I_STEP;
        else if(encoderPos < 0 && ISetSCIHi >= ISET_SC_I_STEP && ISetSCIHi > ISetSCILo)           
          ISetSCIHi -= ISET_SC_I_STEP;   
        break;
      case SC_PARAM_ILO:
        if(encoderPos > 0 && ISetSCILo < ISET_SC_IMAX && ISetSCILo < ISetSCIHi)  
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


  else if(mode == TRIANGLE_CURRENT) {
    int encoderPos = getEncoderMovement();       //encoderPos is either -1, 0 or 1
    if(encoderPos != 0) {                        //Encoder changed
      switch(TCCurrentParam) {
      case TC_PARAM_IHI:
        if(encoderPos > 0 && ISetTCIHi < ISET_TC_IMAX)  
          ISetTCIHi += ISET_TC_I_STEP;
        else if(encoderPos < 0 && ISetTCIHi >= ISET_SC_I_STEP && ISetTCIHi > ISetTCILo)           
          ISetTCIHi -= ISET_TC_I_STEP;   
        break;
      case TC_PARAM_ILO:
        if(encoderPos > 0 && ISetTCILo < ISET_TC_IMAX && ISetTCILo < ISetTCIHi)  
          ISetTCILo += ISET_TC_I_STEP;
        else if(encoderPos < 0 && ISetTCILo >= ISET_TC_I_STEP)           
          ISetTCILo -= ISET_TC_I_STEP;   
        break;
      case TC_PARAM_THI:
        if(encoderPos > 0 && ISetTCTHi < ISET_TC_TMAX)  
          ISetTCTHi += ISET_TC_T_STEP;
        else if(encoderPos < 0 && ISetTCTHi >= ISET_TC_T_STEP)           
          ISetTCTHi -= ISET_TC_T_STEP;   
        break;
      case TC_PARAM_TLO:
      if(encoderPos > 0 && ISetTCTLo < ISET_TC_TMAX)  
          ISetTCTLo += ISET_TC_T_STEP;
        else if(encoderPos < 0 && ISetTCTLo >= ISET_TC_T_STEP)           
          ISetTCTLo -= ISET_TC_T_STEP;  
        break;
      }
    }

    lcd.setCursor(0, 1);
    switch(TCCurrentParam) {
      case TC_PARAM_IHI:
        lcd.write("I");
        lcd.write(byte(3));
        lcdPrintIntWhitespace(ISetTCIHi, 4);
        lcd.print(ISetTCIHi);
        lcd.write("mA");
        break;
        
      case TC_PARAM_ILO:
        lcd.write("I");
        lcd.write(byte(4));
        lcdPrintIntWhitespace(ISetTCILo, 4);
        lcd.print(ISetTCILo);
        lcd.write("mA");
        break;
        
      case TC_PARAM_THI:
        lcd.write("T");
        lcd.write(byte(3));
        lcdPrintIntWhitespace(ISetTCTHi, 4);
        lcd.print(ISetTCTHi);
        lcd.write("mS");   
        break;
           
       case TC_PARAM_TLO:
        lcd.write("T");
        lcd.write(byte(4));
        lcdPrintIntWhitespace(ISetTCTLo, 4);
        lcd.print(ISetTCTLo);
        lcd.write("mS");
        break;
    } 
  }


  else if(mode == SINE_CURRENT) {
  int encoderPos = getEncoderMovement();         //encoderPos is either -1, 0 or 1
    if(encoderPos != 0) {                        //Encoder changed
      switch(SNCCurrentParam) {
      case SNC_PARAM_IHI:
        if(encoderPos > 0 && ISetSNCIHi < ISET_SNC_IMAX)  
          ISetSNCIHi += ISET_SNC_I_STEP;
        else if(encoderPos < 0 && ISetSNCIHi >= ISET_SNC_I_STEP && ISetSNCIHi > ISetSNCILo)           
          ISetSNCIHi -= ISET_SNC_I_STEP;   
        break;
      case SNC_PARAM_ILO:
        if(encoderPos > 0 && ISetSNCILo < ISET_SNC_IMAX && ISetSNCILo < ISetSNCIHi)  
          ISetSNCILo += ISET_SNC_I_STEP;
        else if(encoderPos < 0 && ISetSNCILo >= ISET_SNC_I_STEP)           
          ISetSNCILo -= ISET_SNC_I_STEP;   
        break;
      case SNC_PARAM_T:
        if(encoderPos > 0 && ISetSNCT < ISET_SNC_TMAX)  
          ISetSNCT += ISET_SNC_T_STEP;
        else if(encoderPos < 0 && ISetSNCT >= ISET_SNC_T_STEP)           
          ISetSNCT -= ISET_SNC_T_STEP;   
        break;
      }
    }

    lcd.setCursor(0, 1);
    switch(SNCCurrentParam) {
      case SNC_PARAM_IHI:
        lcd.write("I");
        lcd.write(byte(3));
        lcdPrintIntWhitespace(ISetSNCIHi, 4);
        lcd.print(ISetSNCIHi);
        lcd.write("mA");
        break;
        
      case SNC_PARAM_ILO:
        lcd.write("I");
        lcd.write(byte(4));
        lcdPrintIntWhitespace(ISetSNCILo, 4);
        lcd.print(ISetSNCILo);
        lcd.write("mA");
        break;
        
      case SNC_PARAM_T:
        lcd.write("T:");
        lcdPrintIntWhitespace(ISetSNCT, 4);
        lcd.print(ISetSNCT);
        lcd.write("mS");   
        break;
    } 
  }


  //Update VLoad and ILoad readings
  //Sample every VLOAD_ILOAD_SAMPLE_REFRESH_INTERVAL_MS
  if(currentMillis > LastVLoadILoadSampleMs + VLOAD_ILOAD_SAMPLE_REFRESH_INTERVAL_MS)  {
    LastVLoadILoadSampleMs = currentMillis;
    sampleVLoad(); 
    sampleILoad();
  }

//  //Update values on LCD every UI_REFRESH_INTERVAL_MS
  if(currentMillis > LastVLoadILoadUpdateMs + UI_REFRESH_INTERVAL_MS)  {
    LastVLoadILoadUpdateMs = currentMillis;

//    Serial.print("VLoad=");
//    Serial.print(VLoad);
//    Serial.print(" ILoad=");
//    Serial.println(ILoad);
    printVLoadAndILoad();
  }

  


  //Handle Mode change button press
  if(digitalRead(modeSelectorBtn) == LOW && (currentMillis > modeSelectorBtnLastPressMs + BUTTON_DEBOUNCE_MS) )  {
    modeSelectorBtnLastPressMs = currentMillis;
    handleModeSelection();
  }


  //Handle Output Enable/Disable button press
  if(digitalRead(outputEnableBtn) == LOW && (currentMillis > outputEnableBtnLastPressMs + BUTTON_DEBOUNCE_MS) )  {
    outputEnableBtnLastPressMs = currentMillis;
    handleOutputToggle();
  }


  //Handle Encoder button press (toggle parameter change)
  if(digitalRead(encBtn) == LOW && (currentMillis > encoderBtnLastPressMs + BUTTON_DEBOUNCE_MS) )  {
    encoderBtnLastPressMs = currentMillis;
    handleEncoderButtonPress();
  } 


//A little delay for good measure
delay(100);

}
