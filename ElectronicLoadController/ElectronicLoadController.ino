#define DEBUG

// This optional setting causes Encoder to use more optimized code,
// It must be defined before Encoder.h is included.
#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>

#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "values.h"
#include "lcd.h"
#include "pwm16Bit.h"
#include "iSet.h"
#include "timer2.h"
#include "userActions.h"
#include "misc.h"
#include "eeprom.h"

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
  lcd.createChar(7, degC);


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
  lcd.write("CC");


  //disable eeprom loading quickly
  //EEPROM.put(eepromVarsWrittenAddress, 123);

  //Load EEPROM values
  loadEepromValues();
              analogWrite(11, 100 );

}


void loop() {
  unsigned long currentMillis = millis();
  int encoderPos = getEncoderMovement();       //getEncoderMovement() returns either a negative value, 0 or a positive value
  
  if(mode == CONSTANT_CURRENT) {
    if(encoderPos != 0 || modeJustChanged) {                        //Encoder or mode changed
      modeJustChanged = false;
      ISetVal += ISET_CC_STEP * encoderPos;
      if(encoderPos < 0 && ISetVal >= ISET_CC_STEP) {
          if(ISetVal > ISET_CC_MAX) ISetVal = 0; //ISetVal is unsigned, if pushed lower than zero it'll loop back to Int.MAX!
      } else if(encoderPos > 0 && ISetVal > ISET_CC_MAX) {
          if(ISetVal > ISET_CC_MAX) ISetVal = ISET_CC_MAX;
      }

      lcd.setCursor(0, 1);
      lcdPrintIntWhitespace(ISetVal, 4);
      lcd.print(ISetVal); 
      lcd.write("mA");
    }
  }


  else if(mode == CONSTANT_RESISTANCE) {
    if(encoderPos != 0 || modeJustChanged) {                        //Encoder or mode changed
      modeJustChanged = false;
      ISetVal += ISET_CR_STEP * encoderPos;
      if(encoderPos < 0) {
          if(ISetVal > ISET_CR_MAX || ISetVal < ISET_CR_MIN) ISetVal = ISET_CR_MIN; //ISetVal is unsigned, if pushed lower than zero it'll loop back to Int.MAX!
            
      } else if(encoderPos > 0 && ISetVal > ISET_CR_MAX) {
          if(ISetVal > ISET_CR_MAX) ISetVal = ISET_CR_MAX;
      }
  
      lcd.setCursor(0, 1);
      lcdPrintIntWhitespace(ISetVal, 4);
      lcd.print(ISetVal); 
      lcd.write(byte(0)); //ohm char
    }
  }


  else if(mode == CONSTANT_POWER) {
    if(encoderPos != 0 || modeJustChanged) {                        //Encoder or mode changed
      modeJustChanged = false;
      ISetVal += ISET_CP_STEP * encoderPos;
  
      if(encoderPos < 0 && ISetVal >= ISET_CP_STEP) {
          if(ISetVal > ISET_CP_MAX) ISetVal = 0; //ISetVal is unsigned, if pushed lower than zero it'll loop back to Int.MAX!
            
      } else if(encoderPos > 0 && ISetVal > ISET_CP_MAX) {
          if(ISetVal > ISET_CP_MAX) ISetVal = ISET_CP_MAX;
      }
      
      lcd.setCursor(0, 1);
      lcdPrintIntWhitespace(ISetVal, 5);
      lcd.print(ISetVal); 
      lcd.write("mW");
    }
  }

  else if(mode == SQUARE_CURRENT) {
    if(encoderPos != 0 || modeJustChanged) {                        //Encoder or mode changed
      
      modeJustChanged = false;
      switch(SCCurrentParam) {
        case SC_PARAM_IHI:
          ISetSCIHi += ISET_SC_I_STEP * encoderPos;
  
          if(encoderPos > 0) {
            if(ISetSCIHi > ISET_SC_IMAX) ISetSCIHi = ISET_SC_IMAX;
          } else if(encoderPos < 0) {          
            if(ISetSCIHi > ISET_SC_IMAX || ISetSCIHi <= ISetSCILo) 
              //ISetSCIHi is unsigned, if pushed lower than zero it'll loop back to Int.MAX!
              //If lower than low, set to low+step!
              ISetSCIHi = ISetSCILo + ISET_SC_I_STEP; 
          }
          break;
          
        case SC_PARAM_ILO:
          ISetSCILo += ISET_SC_I_STEP * encoderPos;
          if(encoderPos > 0) {
            if(ISetSCILo >= ISetSCIHi) ISetSCILo = ISetSCIHi-ISET_SC_I_STEP;
          } else if(encoderPos < 0) {          
            if(ISetSCILo > ISET_SC_IMAX) ISetSCILo = 0; //ISetSCILo is unsigned, if pushed lower than zero it'll loop back to Int.MAX!
          }
          break;
          
        case SC_PARAM_THI:
            ISetSCTHi += ISET_SC_T_STEP * encoderPos;
          if(encoderPos > 0 && ISetSCTHi > ISET_SC_TMAX) 
            ISetSCTHi = ISET_SC_TMAX;
          else if(encoderPos < 0 && ISetSCTHi > ISET_SC_TMAX || ISetSCTHi == 0) //ISetSCTHi is unsigned, if pushed lower than zero it'll loop back to Int.MAX!
            ISetSCTHi = ISET_SC_T_STEP;         
          break;
          
        case SC_PARAM_TLO:
          ISetSCTLo += ISET_SC_T_STEP * encoderPos;
          if(encoderPos > 0 && ISetSCTLo > ISET_SC_TMAX) 
            ISetSCTLo = ISET_SC_TMAX;
          else if(encoderPos < 0 && ISetSCTLo > ISET_SC_TMAX || ISetSCTLo == 0) //ISetSCTLo is unsigned, if pushed lower than zero it'll loop back to Int.MAX!
          ISetSCTLo = ISET_SC_T_STEP;
          break;
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
  }

  else if(mode == TRIANGLE_CURRENT) {
    if(encoderPos != 0 || modeJustChanged) {                        //Encoder or mode changed
      
      modeJustChanged = false;
      switch(TCCurrentParam) {
        case TC_PARAM_IHI:
          ISetTCIHi += ISET_TC_I_STEP * encoderPos;
  
          if(encoderPos > 0) {
            if(ISetTCIHi > ISET_TC_IMAX) ISetTCIHi = ISET_TC_IMAX;
          } else if(encoderPos < 0) {          
            if(ISetTCIHi > ISET_TC_IMAX || ISetTCIHi <= ISetTCILo) 
              //ISetTCIHi is unsigned, if pushed lower than zero it'll loop back to Int.MAX!
              //If lower than low, set to low+step!
              ISetTCIHi = ISetTCILo + ISET_TC_I_STEP; 
          }
          break;
          
        case TC_PARAM_ILO:
          ISetTCILo += ISET_TC_I_STEP * encoderPos;
          if(encoderPos > 0) {
            if(ISetTCILo >= ISetTCIHi) ISetTCILo = ISetTCIHi-ISET_TC_I_STEP;
          } else if(encoderPos < 0) {          
            if(ISetTCILo > ISET_TC_IMAX) ISetTCILo = 0; //ISetTCILo is unsigned, if pushed lower than zero it'll loop back to Int.MAX!
          }
          break;
          
        case TC_PARAM_THI:
            ISetTCTHi += ISET_TC_T_STEP * encoderPos;
          if(encoderPos > 0 && ISetTCTHi > ISET_TC_TMAX) 
            ISetTCTHi = ISET_TC_TMAX;
          else if(encoderPos < 0 && ISetTCTHi > ISET_TC_TMAX || ISetTCTHi == 0) //ISetTCTHi is unsigned, if pushed lower than zero it'll loop back to Int.MAX!
            ISetTCTHi = ISET_TC_T_STEP;         
          break;
          
        case TC_PARAM_TLO:
          ISetTCTLo += ISET_TC_T_STEP * encoderPos;
          if(encoderPos > 0 && ISetTCTLo > ISET_TC_TMAX) 
            ISetTCTLo = ISET_TC_TMAX;
          else if(encoderPos < 0 && ISetTCTLo > ISET_TC_TMAX || ISetTCTLo == 0) //ISetTCTLo is unsigned, if pushed lower than zero it'll loop back to Int.MAX!
          ISetTCTLo = ISET_TC_T_STEP;
          break;
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
  }


  else if(mode == SINE_CURRENT) {
    if(encoderPos != 0 || modeJustChanged) {                        //Encoder or mode changed
      modeJustChanged = false;
      
      switch(SNCCurrentParam) {
        case SNC_PARAM_IHI:
          ISetSNCIHi += ISET_SNC_I_STEP * encoderPos;

          if(encoderPos > 0) {
            if(ISetSNCIHi > ISET_SNC_IMAX) ISetSNCIHi = ISET_SNC_IMAX;
          } else if(encoderPos < 0) {          
            if(ISetSNCIHi > ISET_SNC_IMAX || ISetSNCIHi <= ISetSNCILo) 
              //ISetSNCIHi is unsigned, if pushed lower than zero it'll loop back to Int.MAX!
              //If lower than low, set to low+step!
              ISetSNCIHi = ISetSNCILo + ISET_SNC_I_STEP; 
          }
          break;

        case SNC_PARAM_ILO:
          ISetSNCILo += ISET_SNC_I_STEP * encoderPos;
          if(encoderPos > 0) {
            if(ISetSNCILo >= ISetSNCIHi) ISetSNCILo = ISetSNCIHi-ISET_SNC_I_STEP;
          } else if(encoderPos < 0) {          
            if(ISetSNCILo > ISET_SNC_IMAX) ISetSNCILo = 0; //ISetSNCILo is unsigned, if pushed lower than zero it'll loop back to Int.MAX!
          }
          break;

        case SNC_PARAM_T:
          ISetSNCT += ISET_SNC_T_STEP * encoderPos;
          if(encoderPos > 0 && ISetSNCT > ISET_SNC_TMAX) 
            ISetSNCT = ISET_SNC_TMAX;
          else if(encoderPos < 0 && ISetSNCT > ISET_SNC_TMAX || ISetSNCT == 0) //ISetSNCT is unsigned, if pushed lower than zero it'll loop back to Int.MAX!
          ISetSNCT = ISET_SNC_T_STEP;
          break;
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
  }


  //Update VLoad, ILoad and Temp readings
  //Sample every SHORT_INTERVAL_MS
  if(currentMillis > LastShortIntervalMs + SHORT_INTERVAL_MS)  {
    LastShortIntervalMs = currentMillis;
    
    sampleVLoad(); 
    sampleILoad();
  }

  //Update LCD every NORMAL_INTERVAL_MS
  if(currentMillis > LastNormalIntervalMs + NORMAL_INTERVAL_MS)  {
    LastNormalIntervalMs = currentMillis;
    
    printVLoadAndILoad(); 
  }
  

  //Update Temp reading and fan PWM every LONG_INTERVAL_MS
  if(currentMillis > LastLongIntervalMs + LONG_INTERVAL_MS)  {
    LastLongIntervalMs = currentMillis;
    
    sampleTempAndUpdateFan();
    printTemp();
    checkForOverTemp();      
  }

  


  //Handle Mode change button press
  if(digitalRead(modeSelectorBtn) == LOW && (currentMillis > modeSelectorBtnLastPressMs + BUTTON_DEBOUNCE_MS) )  {
    modeSelectorBtnLastPressMs = currentMillis;
    handleModeSelection(true);
  }


  //Handle Output Enable/Disable button press
  if(digitalRead(outputEnableBtn) == LOW && (currentMillis > outputEnableBtnLastPressMs + BUTTON_DEBOUNCE_MS) )  {
    outputEnableBtnLastPressMs = currentMillis;
    
    if(!outputEnabled && currentTemperature < overheatTempVal)
      handleOutputToggle();
    else {
      handleOutputToggle();
    }
  }


  //Handle Encoder button press (toggle parameter change)
  if(digitalRead(encBtn) == LOW && (currentMillis > encoderBtnLastPressMs + BUTTON_DEBOUNCE_MS) )  {
    encoderBtnLastPressMs = currentMillis;
    handleEncoderButtonPress();
  } 

//A little delay for good measure
delay(100);

}
