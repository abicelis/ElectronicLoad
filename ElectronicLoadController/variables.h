#ifdef DEBUG
 #define DEBUG_PRINT(x)     Serial.print (x)
 #define DEBUG_PRINTLN(x)   Serial.println (x)
#else
 #define DEBUG_PRINT(x)
 #define DEBUG_PRINTLN(x) 
#endif


/* GENERAL USE CONSTANTS */
#define CONSTANT_CURRENT        1
#define CONSTANT_RESISTANCE     2
#define CONSTANT_POWER          3
#define SINE_CURRENT            4
#define SQUARE_CURRENT          5
#define TRIANGLE_CURRENT        6

#define MS_PER_CYCLE            1      //Timer2 interrupts at 1KHz (1mS period)
#define BUFFER_SIZE             1      //Buffer size for VSense and ISense (voltageAndCurrentSensors.h)


/*C.Current, C.Resistance and C.Power Modes */
#define ISET_CC_MAX             2500
#define ISET_CC_DEFAULT         250
#define ISET_CC_STEP            25

#define ISET_CR_MIN             5      //Constant Resistance min in ohms
#define ISET_CR_MAX             500    //Constant Resistance max in ohms
#define ISET_CR_DEFAULT         50
#define ISET_CR_STEP            5

#define ISET_CP_MAX             5000    //Constant Power max in mW
#define ISET_CP_DEFAULT         500
#define ISET_CP_STEP            20


/*Square wave variable current mode */
#define SC_PARAM_IHI            10
#define SC_PARAM_ILO            20
#define SC_PARAM_THI            30
#define SC_PARAM_TLO            40

#define ISET_SC_IMAX            1500    //Max current in mA
#define ISET_SC_TMAX            2000    //Max current in mS
#define ISET_SC_IHI_DEFAULT     1000    //Default value for IHI in mA
#define ISET_SC_ILO_DEFAULT     200     //Default value for ILO in mA
#define ISET_SC_THI_DEFAULT     1000    //Time spent @ IHI in mS
#define ISET_SC_TLO_DEFAULT     1000    //Time spent @ ILO in mS
#define ISET_SC_I_STEP          50
#define ISET_SC_T_STEP          100

boolean SCisHi = true;                      //Flag, true when ISet = ISetSCIHi
uint16_t ISetSCIHi = ISET_SC_IHI_DEFAULT;
uint16_t ISetSCILo = ISET_SC_ILO_DEFAULT;
uint16_t ISetSCTHi = ISET_SC_THI_DEFAULT;
uint16_t ISetSCTLo = ISET_SC_TLO_DEFAULT;
byte SCCurrentParam = SC_PARAM_IHI;

/*Triangle wave variable current mode */
#define TC_PARAM_IHI            11
#define TC_PARAM_ILO            21
#define TC_PARAM_THI            31
#define TC_PARAM_TLO            41

#define ISET_TC_IMAX            1500    //Max current in mA
#define ISET_TC_TMAX            2000    //Max current in mS
#define ISET_TC_IHI_DEFAULT     1000    //Default value for IHI in mA
#define ISET_TC_ILO_DEFAULT     200     //Default value for ILO in mA
#define ISET_TC_THI_DEFAULT     1000    //Time spent @ IHI in mS
#define ISET_TC_TLO_DEFAULT     500     //Time spent @ ILO in mS
#define ISET_TC_I_STEP          50
#define ISET_TC_T_STEP          100

boolean TCisHi = true;                      //Flag, true when ISet = ISetTCIHi
uint16_t ISetTCIHi = ISET_TC_IHI_DEFAULT;
uint16_t ISetTCILo = ISET_TC_ILO_DEFAULT;
uint16_t ISetTCTHi = ISET_TC_THI_DEFAULT;
uint16_t ISetTCTLo = ISET_TC_TLO_DEFAULT;
float ISetIStep;
byte TCCurrentParam = TC_PARAM_IHI;





/* ILoad set pin */
const int ISetPin = 10;               //PWM output pin, sets a voltage which in turn sets the current of the load
const int ISetCalibrationPin = A0;    //Analog input pin, used to calibrate ISet
uint16_t ISetOneAmp = -1;             //16bit PWM value, equivalent to 1v @ IsetPin (1A @ ILoad)
uint16_t ISetVal = ISET_CC_DEFAULT;   // The current ISet value (mA)
uint16_t ISetTimeCounter = 0;         //Counter, used in SINE_CURRENT, SQUARE_CURRENT and TRIANGLE_CURRENT
uint16_t ISetTimeOverflowVal;         //Overflow val for ISetTimeCounter

/* VLoad sense pin */
const int VLoadPin = A1;              //10bit VLoad sense pin (Caution! 17V MAX Vin on the voltage divider on this pin) 
                                      //Vin *---[R=7,445K]---* VLoad(Arduino) *---[R=3k]---* GND
float VLoad = 0;                      //Holds VLoad
float VLoadBuffer = 0;                //Buffer for a Smoother VLoad   

/* ILoad sense pin */
const int ILoadPin = A2;              //10bit ILoad sense pin (Measures ILoad current)
int ILoad = 0;                        //Holds ILoad
int ILoadBuffer = 0;                  //Buffer for a Smoother ILoad   

/* Output enabled Pin */
const byte outputEnabledLed = 9;

/* Rotary Encoder */
Encoder myEnc(2, 3);                  //Rotary encoder attached to pins 2 and 3, using interrupts INT0 and INT1.
const byte encBtn = 8;                //Rotary encoder button pin, connected to digital pin 8

/* Mode change and Enable Buttons */
const byte modeSelectorBtn = A5;
const byte outputEnableBtn = A4;

byte mode = CONSTANT_CURRENT;         //Constant Current mode by default
bool outputEnabled = false;           //Flag, sets the output as enabled (@ISetVal) or disabled (0v)



