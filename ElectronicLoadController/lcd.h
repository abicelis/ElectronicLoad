/*
 * LCD
 * Wiring for the LCD:
 * LCD PIN    Arduino pin breadboard    PCB
 * RS         13                        12
 * Enable     12                        13
 * D4         4                         7
 * D5         5                         6
 * D6         6                         5
 * D7         7                         4
 * R/W pin to ground
 * VSS pin to ground
 * VCC pin to 5V
 * 10K variable resistor:
 *    ends to +5V and GND
 *    wiper to LCD VO pin
 */

//PCB rev1
#define LCD_RS                  12
#define LCD_ENABLE              13
#define LCD_D4                  7
#define LCD_D5                  6
#define LCD_D6                  5
#define LCD_D7                  4

////Breadboard
//#define LCD_RS                  13
//#define LCD_ENABLE              12
//#define LCD_D4                  4
//#define LCD_D5                  5
//#define LCD_D6                  6
//#define LCD_D7                  7
LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);


int getLenghtOfFloat(float var)
{
  if(var >= 10)
    return 5;
  else
    return 4;
}


int getLenghtOfInt(int var)
{
  if(var >= 100000)
    return 6; 
  if(var >= 10000)
    return 5; 
  if(var >= 1000)
    return 4; 
  if(var >= 100)
    return 3;  
  else if(var >= 10)
    return 2;
  else
    return 1;
}


/*
 *@Param numberChars: The max space in chars the int can be, if val can be max 999 then numberChars=3
 */
void lcdPrintIntWhitespace(int val, byte numberChars) {
  for(int i = 0; i < numberChars-getLenghtOfInt(val) ; i++) {
    lcd.print(" ");
  }
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


/* Custom LCD chars */
// ohm sign
byte ohm[8] = {
  B00000,
  B01110,
  B10001,
  B10001,
  B10001,
  B01010,
  B11011,
  B00000
};
byte squaree[8] = {
  0b00000,
  0b01110,
  0b01010,
  0b01010,
  0b01010,
  0b01010,
  0b11011,
  0b00000
};
byte triang1[8] = {
  0b00000,
  0b00100,
  0b01010,
  0b10001,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
byte triang2[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b10001,
  0b01010,
  0b00100
};
byte sine1[8] = {
  0b00000,
  0b00000,
  0b01110,
  0b10001,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
byte sine2[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b10001,
  0b01110,
  0b00000
};

byte vLoad[8] = {
  0b10100,
  0b10100,
  0b10100,
  0b01000,
  0b00001,
  0b10000,
  0b10001,
  0b11100
};

byte iLoad[8] = {
  0b11100,
  0b01000,
  0b01000,
  0b11100,
  0b00001,
  0b10000,
  0b10001,
  0b11100
};

byte hi[8] = {
  0b10100,
  0b11100,
  0b10101,
  0b00000,
  0b11101,
  0b01000,
  0b11100,
  0b00000
};

byte lo[8] = {
  0b10000,
  0b10000,
  0b11101,
  0b00000,
  0b11101,
  0b10100,
  0b11100,
  0b00000
};

byte degC[8] = {
  0b11100,
  0b10100,
  0b11100,
  0b00000,
  0b00111,
  0b00100,
  0b00100,
  0b00111
};

//byte degF[8] = {
//  0b11100,
//  0b10100,
//  0b11100,
//  0b00000,
//  0b00111,
//  0b00100,
//  0b00110,
//  0b00100
//};
