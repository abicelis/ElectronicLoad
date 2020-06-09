/* GENERAL USE CONSTANTS */
#define CONSTANT_CURRENT        1
#define CONSTANT_RESISTANCE     2
#define CONSTANT_POWER          3
#define SINE_CURRENT            4
#define SQUARE_CURRENT          5
#define TRIANGLE_CURRENT        6

#define MS_PER_CYCLE            1      //Timer2 interrupts at 1KHz (1mS period)
#define BUFFER_SIZE             5      //Buffer size for VSense and ISense (voltageAndCurrentSensors.h)



/* MISC */
unsigned int LONG_INTERVAL_MS = 1000;
unsigned int NORMAL_INTERVAL_MS = 500;
unsigned int SHORT_INTERVAL_MS = 50;
unsigned long LastLongIntervalMs = 0;
unsigned long LastNormalIntervalMs = 0;
unsigned long LastShortIntervalMs = 0;

unsigned int BUTTON_DEBOUNCE_MS = 500;
unsigned long modeSelectorBtnLastPressMs = 0;
unsigned long outputEnableBtnLastPressMs = 0;
unsigned long encoderBtnLastPressMs = 0;


bool forceLCDRedraw = true;                  //Set to true, then immediately to false somewhere on loop().  Used to force a redraw.



/* STATE */
byte mode = CONSTANT_CURRENT;         //Constant Current mode by default
bool outputEnabled = false;           //Flag, sets the output as enabled (@ISetVal) or disabled (0v)




/*C.Current, C.Resistance and C.Power Modes */
#define ISET_CC_MAX             3000
#define ISET_CC_DEFAULT         200
#define ISET_CC_STEP            25

#define ISET_CR_MIN             5      //Constant Resistance min in ohms
#define ISET_CR_MAX             500    //Constant Resistance max in ohms
#define ISET_CR_DEFAULT         50
#define ISET_CR_STEP            1

#define ISET_CP_MAX             10000    //Constant Power max in mW
#define ISET_CP_DEFAULT         500
#define ISET_CP_STEP            25




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
#define ISET_SC_I_STEP          25
#define ISET_SC_T_STEP          25

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
#define ISET_TC_I_STEP          25
#define ISET_TC_T_STEP          25

boolean TCisHi = true;                      //Flag, true when ISet = ISetTCIHi
uint16_t ISetTCIHi = ISET_TC_IHI_DEFAULT;
uint16_t ISetTCILo = ISET_TC_ILO_DEFAULT;
uint16_t ISetTCTHi = ISET_TC_THI_DEFAULT;
uint16_t ISetTCTLo = ISET_TC_TLO_DEFAULT;
byte TCCurrentParam = TC_PARAM_IHI;




/*Sine wave variable current mode */
#define SNC_PARAM_IHI            15
#define SNC_PARAM_ILO            25
#define SNC_PARAM_T              35

#define ISET_SNC_IMAX            1500    //Max current in mA
#define ISET_SNC_TMAX            2000    //Max current in mS
#define ISET_SNC_IHI_DEFAULT     1000    //Default value for IHI in mA
#define ISET_SNC_ILO_DEFAULT     200     //Default value for ILO in mA
#define ISET_SNC_T_DEFAULT       1000    //Sine wave period
#define ISET_SNC_I_STEP          25
#define ISET_SNC_T_STEP          25

uint16_t ISetSNCIHi = ISET_SNC_IHI_DEFAULT;
uint16_t ISetSNCILo = ISET_SNC_ILO_DEFAULT;
uint16_t ISetSNCT   = ISET_SNC_T_DEFAULT;
byte SNCCurrentParam = SNC_PARAM_IHI;
int sine360[] = {0, 1, 3, 5, 6, 8, 10, 12, 13, 15, 17, 19, 20, 22, 24, 25, 27, 29, 30, 32, 34, 35, 37, 39, 40, 42, 43, 45, 46, 48, 49, 51, 52, 54, 55, 57, 58, 60, 61, 62, 64, 65, 66, 68, 69, 70, 71, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 89, 90, 91, 92, 92, 93, 93, 94, 95, 95, 96, 96, 97, 97, 97, 98, 98, 98, 99, 99, 99, 99, 99, 99, 99, 99, 100, 99, 99, 99, 99, 99, 99, 99, 99, 98, 98, 98, 97, 97, 97, 96, 96, 95, 95, 94, 93, 93, 92, 92, 91, 90, 89, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 71, 70, 69, 68, 66, 65, 64, 62, 61, 60, 58, 57, 55, 54, 52, 51, 49, 48, 46, 45, 43, 42, 40, 39, 37, 35, 34, 32, 30, 29, 27, 25, 24, 22, 20, 19, 17, 15, 13, 12, 10, 8, 6, 5, 3, 1, 0, -1, -3, -5, -6, -8, -10, -12, -13, -15, -17, -19, -20, -22, -24, -25, -27, -29, -30, -32, -34, -35, -37, -39, -40, -42, -43, -45, -46, -48, -50, -51, -52, -54, -55, -57, -58, -60, -61, -62, -64, -65, -66, -68, -69, -70, -71, -73, -74, -75, -76, -77, -78, -79, -80, -81, -82, -83, -84, -85, -86, -87, -88, -89, -89, -90, -91, -92, -92, -93, -93, -94, -95, -95, -96, -96, -97, -97, -97, -98, -98, -98, -99, -99, -99, -99, -99, -99, -99, -99, -100, -99, -99, -99, -99, -99, -99, -99, -99, -98, -98, -98, -97, -97, -97, -96, -96, -95, -95, -94, -93, -93, -92, -92, -91, -90, -89, -89, -88, -87, -86, -85, -84, -83, -82, -81, -80, -79, -78, -77, -76, -75, -74, -73, -71, -70, -69, -68, -66, -65, -64, -62, -61, -60, -58, -57, -55, -54, -52, -51, -50, -48, -46, -45, -43, -42, -40, -39, -37, -35, -34, -32, -30, -29, -27, -25, -24, -22, -20, -19, -17, -15, -13, -12, -10, -8, -6, -5, -3, -1};
float SNCWaveCenter;         //mA's when wave is at sine(0), sine(180), sine(360).
float SNCWaveAmplitude;      //mA's from wave's canter to its max val.




/* ILoad set pin */
const int ISetPin = 10;               //PWM output pin, sets a voltage which in turn sets the current of the load
const int ISetCalibrationPin = A0;    //Analog input pin, used to calibrate ISet
uint16_t ISetOneAmp = -1;             //16bit PWM value, equivalent to 1v @ IsetPin (1A @ ILoad)
uint16_t ISetVal = ISET_CC_DEFAULT;   // The current ISet value (mA)
uint16_t ISetTimeCounter = 0;         //Counter, used in SINE_CURRENT, SQUARE_CURRENT and TRIANGLE_CURRENT
uint16_t ISetTimeOverflowVal;         //Overflow val for ISetTimeCounter
float ISetIStep;



/* VLoad sense pin */
const int VLoadPin = A1;              //10bit VLoad sense pin
                                      //Vin *---[R1]---* VLoadPin(Arduino) *---[R2]---* GND
float VLoad = 0;                      //Holds VLoad
float VLoadBuffer = 0;                //Buffer for a Smoother VLoad   
float VLoadTweak = 0.0f;              //Default value for tweaking VLoad, saved/loaded to/from EEPROM, in volts


/* ILoad sense pin */
const int ILoadPin = A2;              //10bit ILoad sense pin (Measures ILoad current by measuring the voltage drop on the 1ohm resistor, so V=I*1, V=I)
int ILoad = 0;                        //Holds ILoad
int ILoadBuffer = 0;                  //Buffer for a Smoother ILoad   
int ILoadTweak = 0;                   //Default value for tweaking ILoad, saved/loaded to/from EEPROM, in mA



/* Rotary Encoder */
Encoder myEnc(3, 2);                  //Rotary encoder attached to pins 2 and 3, using interrupts INT0 and INT1.
const byte encBtn = 8;                //Rotary encoder button pin, connected to digital pin 8



/* Temps, Fan control and LM35 */
const int fanPin = 9;                 //PWM output pin, sets a voltage which in turn sets the speed of the fan!
const byte temperatureSensorPin = A3; //Where the LM35 out is hooked. 
int currentTemperature = 0;           //Current temperature ADC value. 150c is ~306. See below.
                                      //LM35 can measure up to 150C, thats 10mV per degree, starting from 0. So 0 to 1500mV. For a 10-bit ADC, 1500mV is ~307 of 1024
const int tempValMinFanSpin = 61;     //At ~30C, we should spin the fan at min speed. Thats 300mV. For a 10-bit ADC, that's ~61 of 1024
const int tempValMaxFanSpin = 102;    //At ~50C, we should spin the fan at max speed. Thats 700mV. For a 10-bit ADC, that's ~102 of 1024
const int overheatTempVal = 184;      //At ~90C, we're overheating. For a 10-bit ADC, 900mV is ~184 of 1024
const int OVERTEMP_DELAY_MS = 10000;   //Number of ms to show the OVERTEMP message

/* Mode change and Enable Buttons */
const byte modeSelectorBtn = A5;
const byte outputEnableBtn = A4;



#ifdef DEBUG
 #define DEBUG_PRINT(x)     Serial.print (x)
 #define DEBUG_PRINTLN(x)   Serial.println (x)
#else
 #define DEBUG_PRINT(x)
 #define DEBUG_PRINTLN(x) 
#endif
