void setupTimer2() { 
  cli();//stop interrupts

  //set timer2 interrupt at 500Hz
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register for 1KHz increments
  OCR2A = 249;// = (16*10^6) / (1000*64) - 1 (must be <256)
              //  16Mhz Clock / (Desired ISR Freq*Prescaler) - 1
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set CS22 bit for 256 prescaler
  TCCR2B |= (1 << CS22);   
  // enable timer compare interrupt
  // TIMSK2 |= (1 << OCIE2A);

  sei();//allow interrupts
}

void enableTimer2(){
    // enable timer compare interrupt
    TIMSK2 |= (1 << OCIE2A);
    DEBUG_PRINTLN("TIMER2 ENABLED");
}

void disableTimer2(){
    // disable timer compare interrupt
    TIMSK2 = 0;
    DEBUG_PRINTLN("TIMER2 DISABLED");
}

ISR(TIMER2_COMPA_vect){
  
  //Handle ISetVal changes on CC, CR and CP modes
  if(mode == CONSTANT_CURRENT || mode == CONSTANT_RESISTANCE || mode == CONSTANT_POWER){
    writeISet(ISetVal);
  }
  
  else {  //SQUARE_CURRENT, TRIANGLE_CURRENT or SINE_CURRENT
    
    if(mode == SQUARE_CURRENT) {
      if(ISetTimeCounter >= ISetTimeOverflowVal) {
        if(SCisHi){
          digitalWrite(outputEnabledLed, LOW);
          ISetTimeOverflowVal = ISetSCTLo;
          ISetVal = ISetSCILo;
        } else {     
          digitalWrite(outputEnabledLed, HIGH);
          ISetTimeOverflowVal = ISetSCTHi;
          ISetVal = ISetSCIHi;
        }
          SCisHi = !SCisHi;
          ISetTimeCounter = 0;
          writeISet(ISetVal);
      }

      
    } else if(mode == TRIANGLE_CURRENT) {
      if(ISetTimeCounter >= ISetTimeOverflowVal) {  //If counter overflowed, change wave slope
        
        if(TCisHi){
          digitalWrite(outputEnabledLed, LOW);
          ISetTimeOverflowVal = ISetTCTLo;

          if(ISetTCTLo == 0)     //This is here to avoid a div/0 below
            ISetVal = ISetTCILo;
           else 
            ISetIStep = ((float)(ISetTCIHi - ISetTCILo)/ISetTCTLo) * MS_PER_CYCLE;
        } else { 
          digitalWrite(outputEnabledLed, HIGH);
          ISetTimeOverflowVal = ISetTCTHi;

          if(ISetTCTHi == 0)     //This is here to avoid a div/0 below
            ISetVal = ISetTCILo;
           else  
            ISetIStep = ((float)(ISetTCIHi - ISetTCILo)/ISetTCTHi) * MS_PER_CYCLE;
          
        }
        
        TCisHi = !TCisHi;
        ISetTimeCounter = 0;
        
      } else {
          if(TCisHi)
            ISetVal = ISetTCILo + (ISetIStep * ISetTimeCounter);
           else 
            ISetVal = ISetTCIHi - (ISetIStep * ISetTimeCounter);
          
      }   
      writeISet(ISetVal);
    } else if(mode == SINE_CURRENT) {
      if(ISetTimeCounter >= ISetTimeOverflowVal) {  //If counter overflowed, recalculate wave
        ISetTimeOverflowVal = ISetSNCT;
        ISetIStep = (360.0/ISetSNCT) * MS_PER_CYCLE;
        SNCWaveAmplitude = ((float)ISetSNCIHi - ISetSNCILo)/2;
        SNCWaveCenter = ISetSNCILo + SNCWaveAmplitude;
        ISetTimeCounter = 0;
      } else {
        int aux = sine360[(int)(ISetIStep * ISetTimeCounter)];
        ISetVal = SNCWaveCenter + (SNCWaveAmplitude * ((float)aux/100));
         
        if(aux > 0)
          digitalWrite(outputEnabledLed, HIGH);
        else
          digitalWrite(outputEnabledLed, LOW);
      }   
      writeISet(ISetVal);
    }
  }

    
  ISetTimeCounter+=MS_PER_CYCLE;    //increment time counter
}

